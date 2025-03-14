// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WeaponAsset.h"

#include "NiagaraComponent.h"

#include "MyProject/Actors/BaseClass/A_RangeWeapon.h"
#include "MyProject/Components/Weapon/C_RangeWeapon.h"
#include "MyProject/Components/Weapon/C_ThrowWeapon.h"
#include "MyProject/Components/Weapon/C_Weapon.h"
#include "MyProject/DataAsset/DA_RangeWeapon.h"
#include "MyProject/DataAsset/DA_ThrowWeapon.h"
#include "MyProject/DataAsset/DA_Weapon.h"
#include "MyProject/Frameworks/Subsystems/SS_EventGameInstance.h"

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

	UDA_Weapon* WeaponAsset = GetAsset<UDA_Weapon>();
	check(WeaponAsset);

	const AActor* Actor = GetOwner();
	if ( !Actor )
	{
		Actor = Cast<AActor>(GetOuter());
	}
	check( Actor != nullptr );
	
	if ( UC_Weapon* WeaponComponent = Actor->GetComponentByClass<UC_Weapon>() )
	{
		WeaponComponent->UpdateFrom( WeaponAsset );
	}
}


// Called when the game starts
void UC_WeaponAsset::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


