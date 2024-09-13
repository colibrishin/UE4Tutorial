// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Buy.h"

#include "MyWeaponStatComponent.h"
#include "MyProject/MyAimableWeapon.h"
#include "MyProject/Private/Utilities.hpp"
#include "MyProject/Private/CommonBuy.hpp"
#include "MyProject/MyCharacter.h"
#include "MyProject/MyGrenade.h"
#include "MyProject/MyMeleeWeapon.h"


// Sets default values for this component's properties
UC_Buy::UC_Buy()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	SetIsReplicatedByDefault(true);
}

void UC_Buy::BuyWeapon(AMyCharacter* RequestCharacter, const int32 WeaponID) const
{
	if (!IsValid(RequestCharacter))
	{
		LOG_FUNC(LogTemp, Error, "Player is invalid");
		return;
	}

	if (!ValidateBuyRequest(WeaponID, RequestCharacter))
	{
		LOG_FUNC(LogTemp, Error, "BuyWeapon_Validate failed");
		return;
	}

	if (IsValid(RequestCharacter))
	{
		Server_BuyWeapon(RequestCharacter, WeaponID);
	}
}

void UC_Buy::ProcessBuy(AMyCharacter* RequestCharacter, const int32 WeaponID) const
{
	const auto& WeaponData        = GetRowData<FMyCollectableData>(this, WeaponID);
	const auto& WeaponAsset       = Cast<UMyWeaponDataAsset>(WeaponData->CollectableDataAsset);
	const auto& CharacterLocation = RequestCharacter->GetActorLocation();

	if (!WeaponAsset)
	{
		LOG_FUNC(LogTemp, Error, "Invalid collectable information, possibly not a weapon");
		return;
	}
	
	RequestCharacter->GetPlayerState<AMyPlayerState>()->AddMoney
		(
		 -WeaponAsset->GetWeaponStat().Price
		);

	static const TMap<EMyWeaponType, TSubclassOf<AMyWeapon>> TypeMapping
	{
			{EMyWeaponType::Range, AMyAimableWeapon::StaticClass()},
			{EMyWeaponType::Melee, AMyMeleeWeapon::StaticClass()},
			{EMyWeaponType::Throwable, AMyGrenade::StaticClass()}
	};

	static FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AMyWeapon* GeneratedWeapon = GetWorld()->SpawnActor<AMyWeapon>(
		TypeMapping[WeaponAsset->GetWeaponStat().WeaponType],
		CharacterLocation,
		FRotator::ZeroRotator,
		ActorSpawnParameters);

	LOG_FUNC_PRINTF(LogTemp, Warning, "Buying Weapon: %s", *WeaponAsset->GetWeaponStat().Name);
	
	GeneratedWeapon->GetWeaponStatComponent()->SetID(WeaponID);
	GeneratedWeapon->UpdateAsset();
	
	if (IsValid(GeneratedWeapon))
	{
		GeneratedWeapon->SetOwner(RequestCharacter->GetOuter());
		GeneratedWeapon->SetReplicateMovement(true);
		GeneratedWeapon->SetReplicates(true);
		GeneratedWeapon->Server_Interact(RequestCharacter);
	}
}

void UC_Buy::Server_BuyWeapon_Implementation(AMyCharacter* RequestCharacter, const int32 WeaponID) const
{
	if (!ValidateBuyRequest(WeaponID, RequestCharacter))
	{
		LOG_FUNC(LogTemp, Error, "BuyWeapon_Validate failed");
		return;
	}

	if (!IsValid(RequestCharacter))
	{
		LOG_FUNC(LogTemp, Error, "Player is invalid");
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
void UC_Buy::TickComponent(float DeltaTime , ELevelTick TickType ,
                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	// ...
}

