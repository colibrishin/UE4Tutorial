// Fill out your copyright notice in the Description page of Project Settings.


#include "C_RangeWeapon.h"

#include "Camera/CameraComponent.h"

#include "Engine/DamageEvents.h"

#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Private/Utilities.hpp"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UC_RangeWeapon::UC_RangeWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	OnAttack.AddUniqueDynamic(this, &UC_RangeWeapon::Fire);
}


// Called when the game starts
void UC_RangeWeapon::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UC_RangeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UC_RangeWeapon, RecoiledNormal);
}

FVector UC_RangeWeapon::ApplyRecoil(const FVector& InNormal, const FVector& InRightVector, const FVector& InUpVector) const
{
	FVector     Normal = InNormal;
	const float HModifier      = HSpread->GetFloatValue((float)GetConsecutiveShot() / (float)GetAmmoPerClip());
	const float VModifier      = VSpread->GetFloatValue((float)GetConsecutiveShot() / (float)GetAmmoPerClip());

	const float HDegree = FMath::RadiansToDegrees(FMath::Sin(HModifier));
	const float VDegree = FMath::RadiansToDegrees(FMath::Sin(VModifier));

	Normal = Normal.RotateAngleAxis(HDegree, -InRightVector);
	Normal = Normal.RotateAngleAxis(VDegree, InUpVector);

	return Normal;
}

void UC_RangeWeapon::Fire()
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}
	
	const UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>();
	const UCameraComponent* CameraComponent = PickUpComponent->GetOwner()->GetAttachParentActor()->GetComponentByClass<UCameraComponent>();

	if (bHitscan)
	{
		RecoiledNormal = 
			ApplyRecoil(CameraComponent->GetForwardVector(), CameraComponent->GetRightVector(), CameraComponent->GetUpVector());
		
		DoHitscan(RecoiledNormal);
	}
}

void UC_RangeWeapon::DoHitscan(const FVector& InRecoiledNormal)
{
	// Weapon -> Character;
	const UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>();
	const UCameraComponent* Cam = PickUpComponent->GetOwner()->GetAttachParentActor()->GetComponentByClass<UCameraComponent>();
	const FVector ShotPosition = Cam->GetComponentLocation();
	const FVector EndPosition = ShotPosition + (InRecoiledNormal * Range);
	FHitResult OutHitResult{};

	FCollisionQueryParams QueryParams;
	// Weapon;
	QueryParams.AddIgnoredActor(GetOwner());
	// Character;
	QueryParams.AddIgnoredActor(GetOwner()->GetOwner());

	LOG_FUNC_PRINTF(LogWeaponComponent, Log, "Hitscaning with normal %s, Client? : %d", *InRecoiledNormal.ToString(), GetNetMode() == NM_Client);
	
	if (GetWorld()->LineTraceSingleByChannel(OutHitResult, ShotPosition, EndPosition, ECC_Visibility, QueryParams))
	{
#if WITH_EDITOR
		if (!IsDummy())
		{
			DrawDebugLine(GetWorld(), ShotPosition, OutHitResult.Location, FColor::Red, false, 2.f);
			DrawDebugBox(GetWorld(), OutHitResult.Location, FVector::OneVector, FColor::Yellow, false, 2.f);
		}
#endif
		if (AA_Character* HitCharacter = Cast<AA_Character>(OutHitResult.GetActor()))
		{
			// todo: utilize the damage event
			const FDamageEvent DamageEvent{};
			HitCharacter->TakeDamage(Damage, DamageEvent, HitCharacter->GetController(), GetOwner());
		}
		else
		{
			// todo: bullet hole;
		}
	}
}
