// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "MyProject/Actors/BaseClass/A_Character.h"

#include "C_Buy.generated.h"

class UC_PickUp;
DECLARE_LOG_CATEGORY_EXTERN(LogBuyComponent , Log , All);

class AA_Character;

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_Buy : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_Buy();

	void BuyWeapon(AA_Character* RequestCharacter, const int32 WeaponID) const;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ProcessBuy(AA_Character* RequestCharacter , int32 WeaponID) const;

	UFUNCTION(Server, Reliable)
	void Server_BuyWeapon(AA_Character* RequestCharacter , int32 WeaponID) const;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime , ELevelTick TickType ,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
