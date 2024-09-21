// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WeaponAsset.h"

#include "NiagaraComponent.h"

#include "MyProject/Actors/BaseClass/A_Weapon.h"
#include "MyProject/Components/Weapon/C_RangeWeapon.h"
#include "MyProject/Components/Weapon/C_ThrowWeapon.h"
#include "MyProject/Components/Weapon/C_Weapon.h"
#include "MyProject/DataAsset/DA_RangeWeapon.h"
#include "MyProject/DataAsset/DA_ThrowWeapon.h"
#include "MyProject/DataAsset/DA_Weapon.h"

DEFINE_LOG_CATEGORY(LogWeaponAssetComponent);

// Sets default values for this component's properties
UC_WeaponAsset::UC_WeaponAsset()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UC_WeaponAsset::ApplyAsset()
{
	Super::ApplyAsset();

	const UDA_Weapon* WeaponAsset = GetAsset<UDA_Weapon>();
	ensure(WeaponAsset);
	
	if (UC_Weapon* WeaponComponent = GetOwner()->GetComponentByClass<UC_Weapon>())
	{
		WeaponComponent->WeaponType = WeaponAsset->GetWeaponType();
		WeaponComponent->AttackSound = WeaponAsset->GetAttackSound();
		WeaponComponent->ReloadSound = WeaponAsset->GetReloadSound();
		WeaponComponent->AttackRate = WeaponAsset->GetAttackRate();
		WeaponComponent->AttackRate = WeaponAsset->GetAttackRate();
		WeaponComponent->bCanSpray = WeaponAsset->GetSpray();
		WeaponComponent->AmmoPerClip = WeaponAsset->GetMaxAmmo();
		WeaponComponent->TotalAmmo = WeaponAsset->GetMaxAmmo() * WeaponAsset->GetMagazine();
		WeaponComponent->Range = WeaponAsset->GetRange();
		WeaponComponent->Damage = WeaponAsset->GetDamage();
		WeaponComponent->ReloadTime = WeaponAsset->GetReloadTime();

		WeaponComponent->ReloadClip();

		switch (WeaponAsset->GetWeaponType())
		{
		case EMyWeaponType::Range:
			{
				const UDA_RangeWeapon* RangeAsset = GetAsset<UDA_RangeWeapon>();
				check(RangeAsset);
			
				if (UC_RangeWeapon* RangeWeapon = Cast<UC_RangeWeapon>(WeaponComponent))
				{
					RangeWeapon->bAimable = RangeAsset->GetAimable();
					RangeWeapon->VSpread = RangeAsset->GetVSpread();
					RangeWeapon->HSpread = RangeAsset->GetHSpread();
					RangeWeapon->bHitscan = RangeAsset->GetHitscan();
				}

				if (const AA_Weapon* Weapon = Cast<AA_Weapon>(GetOwner()))
				{
					Weapon->BulletTrailComponent->SetAsset(RangeAsset->GetBulletTrail());
				}

				break;
			}
		case EMyWeaponType::Melee: break;
		case EMyWeaponType::Throwable:
			{
				const UDA_ThrowWeapon* ThrowAsset = GetAsset<UDA_ThrowWeapon>();
				check(ThrowAsset);

				if (UC_ThrowWeapon* ThrowWeapon = Cast<UC_ThrowWeapon>(WeaponComponent))
				{
					ThrowWeapon->CookingTime = ThrowAsset->GetCookingTime();
					ThrowWeapon->ThrowForce = ThrowAsset->GetThrowForce();
					ThrowWeapon->ThrowForceMultiplier = ThrowAsset->GetThrowMultiplier();
				}
				
				break;
			}
		case EMyWeaponType::Unknown:
		default:
			// Unknown weapon type caught
			ensure(false);
			break;
		}
	}
}


// Called when the game starts
void UC_WeaponAsset::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


