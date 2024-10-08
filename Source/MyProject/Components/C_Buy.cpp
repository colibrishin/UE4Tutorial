// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Buy.h"

#include "Asset/C_CollectableAsset.h"
#include "Asset/C_WeaponAsset.h"

#include "MyProject/Actors/BaseClass/A_RangeWeapon.h"
#include "MyProject/Private/Utilities.hpp"
#include "MyProject/Private/CommonBuy.hpp"
#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Actors/BaseClass/A_ThrowWeapon.h"
#include "MyProject/Actors/BaseClass/A_Weapon.h"
#include "MyProject/DataAsset/DA_Weapon.h"

DEFINE_LOG_CATEGORY(LogBuyComponent);

// Sets default values for this component's properties
UC_Buy::UC_Buy()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	SetIsReplicatedByDefault(true);
}

void UC_Buy::BuyWeapon(AA_Character* RequestCharacter, const int32 WeaponID) const
{
	if (!IsValid(RequestCharacter))
	{
		LOG_FUNC(LogBuyComponent, Error, "Player is invalid");
		return;
	}

	if (!ValidateBuyRequest(WeaponID, RequestCharacter))
	{
		LOG_FUNC(LogBuyComponent, Error, "BuyWeapon_Validate failed");
		return;
	}

	if (IsValid(RequestCharacter))
	{
		Server_BuyWeapon(RequestCharacter, WeaponID);
	}
}

void UC_Buy::ProcessBuy(AA_Character* RequestCharacter, const int32 WeaponID) const
{
	const auto& WeaponData        = GetWorld()->GetSubsystem<USS_World>()->GetRowData<FBaseAssetRow>(WeaponID);
	const auto& WeaponAsset       = Cast<UDA_Weapon>(WeaponData->AssetToLink);
	ensureAlwaysMsgf(WeaponAsset, TEXT("Asset is not link to weapon asset"));
	const auto& CharacterLocation = RequestCharacter->GetActorLocation();
	
	RequestCharacter->GetPlayerState<AMyPlayerState>()->AddMoney
		(
		 -WeaponAsset->GetPrice()
		);

	static TMap<EMyWeaponType, TSubclassOf<AA_Weapon>> WeaponClassMap
	{
		{EMyWeaponType::Range, AA_RangeWeapon::StaticClass()},
		{EMyWeaponType::Throwable, AA_ThrowWeapon::StaticClass()}
	};

	TSubclassOf<AA_Weapon> WeaponType = AA_Weapon::StaticClass();

	if (WeaponClassMap.Contains(WeaponAsset->GetWeaponType()))
	{
		WeaponType = WeaponClassMap[WeaponAsset->GetWeaponType()]; 
	}

	LOG_FUNC_PRINTF(LogTemp, Warning, "Spawning template weapon: %s", *WeaponAsset->GetAssetName());
	
	const FTransform Transform {FQuat::Identity, CharacterLocation, FVector::OneVector};
	AA_Weapon* GeneratedWeapon = GetWorld()->SpawnActorDeferred<AA_Weapon>(
		WeaponType,
		Transform,
		RequestCharacter,
		RequestCharacter,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
		ESpawnActorScaleMethod::OverrideRootScale);

	GeneratedWeapon->GetAssetComponent()->SetID(WeaponID);
	GeneratedWeapon->FetchAsset();
	UGameplayStatics::FinishSpawningActor(GeneratedWeapon, Transform);
	
	if (const UC_PickUp* PickUpComponent = GeneratedWeapon->GetPickUpComponent())
	{
		PickUpComponent->OnObjectPickUp.Broadcast(RequestCharacter, true);
	}
}

void UC_Buy::Server_BuyWeapon_Implementation(AA_Character* RequestCharacter, const int32 WeaponID) const
{
	if (!ValidateBuyRequest(WeaponID, RequestCharacter))
	{
		LOG_FUNC(LogBuyComponent, Error, "BuyWeapon_Validate failed");
		return;
	}

	if (!IsValid(RequestCharacter))
	{
		LOG_FUNC(LogBuyComponent, Error, "Player is invalid");
		return;
	}

	ProcessBuy(RequestCharacter, WeaponID);
}

// Called when the game starts
void UC_Buy::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UC_Buy::TickComponent(float                                 DeltaTime , ELevelTick TickType ,
                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	// ...
}

