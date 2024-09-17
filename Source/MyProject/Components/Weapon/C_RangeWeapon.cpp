// Fill out your copyright notice in the Description page of Project Settings.


#include "C_RangeWeapon.h"

#include "Camera/CameraComponent.h"

#include "Engine/DamageEvents.h"

#include "MyProject/Actors/BaseClass/A_Character.h"


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

void UC_RangeWeapon::Server_Attack_Implementation()
{
	if (!bFiring)
	{
		// Weapon -> Character -> Character Camera component;
		Normal = GetOwner()->GetOwner()->GetComponentByClass<UCameraComponent>()->GetForwardVector();
	}
	
	Super::Server_Attack_Implementation();

	if (bHitscan)
	{
		DoHitscan(ApplyRecoil(Normal));
	}
}

void UC_RangeWeapon::Server_StopAttack_Implementation()
{
	Super::Server_StopAttack_Implementation();
}

FVector UC_RangeWeapon::ApplyRecoil(const FVector& InNormal) const
{
	FVector     RecoiledNormal = InNormal;
	constexpr float North          = PI / 2;
	const float HModifier      = HSpread->GetFloatValue(GetConsecutiveShot() / GetAmmoPerClip()) + North;
	const float VModifier      = VSpread->GetFloatValue(GetConsecutiveShot() / GetAmmoPerClip()) + North;
	
	RecoiledNormal = RecoiledNormal.RotateAngleAxis(FMath::RadiansToDegrees(HModifier), FVector::RightVector);
	RecoiledNormal = RecoiledNormal.RotateAngleAxis(FMath::RadiansToDegrees(VModifier), FVector::UpVector);

	return RecoiledNormal;
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
