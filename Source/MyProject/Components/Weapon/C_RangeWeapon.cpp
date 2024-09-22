// Fill out your copyright notice in the Description page of Project Settings.


#include "C_RangeWeapon.h"

#include "Camera/CameraComponent.h"

#include "Engine/DamageEvents.h"

#include "MyProject/Actors/BaseClass/A_Character.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UC_RangeWeapon::UC_RangeWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
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

void UC_RangeWeapon::AttackImplementation()
{
	if (!bFiring)
	{
		// Weapon -> Character -> Character Camera component;
		if (const UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>())
		{
			RecoiledNormal = PickUpComponent->GetAttachParentActor()->GetComponentByClass<UCameraComponent>()->GetForwardVector();
		}
	}

	Super::AttackImplementation();

	if (bHitscan)
	{
		RecoiledNormal = ApplyRecoil(RecoiledNormal);
		DoHitscan(RecoiledNormal);
	}
}

void UC_RangeWeapon::StopAttackImplementation()
{
	Super::StopAttackImplementation();
}

FVector UC_RangeWeapon::ApplyRecoil(const FVector& InNormal) const
{
	FVector     Normal = InNormal;
	constexpr float North          = PI / 2;
	const float HModifier      = HSpread->GetFloatValue(GetConsecutiveShot() / GetAmmoPerClip()) + North;
	const float VModifier      = VSpread->GetFloatValue(GetConsecutiveShot() / GetAmmoPerClip()) + North;
	
	Normal = Normal.RotateAngleAxis(FMath::RadiansToDegrees(HModifier), FVector::RightVector);
	Normal = Normal.RotateAngleAxis(FMath::RadiansToDegrees(VModifier), FVector::UpVector);

	return Normal;
}

void UC_RangeWeapon::DoHitscan(const FVector& InRecoiledNormal)
{
	// Weapon -> Character;
	const FVector ShotPosition = GetOwner()->GetOwner()->GetActorLocation();
	const FVector EndPosition = ShotPosition + (InRecoiledNormal * Range);
	FHitResult OutHitResult{};

	FCollisionQueryParams QueryParams;
	// Weapon;
	QueryParams.AddIgnoredActor(GetOwner());
	// Character;
	QueryParams.AddIgnoredActor(GetOwner()->GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(OutHitResult, ShotPosition, EndPosition, ECC_Visibility, QueryParams))
	{
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
