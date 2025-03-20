// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"

#include "C_PickUp.generated.h"

class UInputMappingContext;
class UInputAction;
class IPickingUp;
class AMyCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectPickUp, TScriptInterface<IPickingUp>, InCaller, const bool, bCallPickUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectDrop, TScriptInterface<IPickingUp>, InCaller, const bool, bCallDrop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnObjectPickUpSpawned , AActor* , InSpawnedActor );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnObjectDropSpawned , AActor* , InSpawnedActor );
DECLARE_LOG_CATEGORY_EXTERN(LogPickUp, Log, All);

enum class EPickUp : uint8_t 
{
	PickUp,
	Drop
};

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_PickUp : public USphereComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_PickUp();

	FOnObjectPickUp OnObjectPickUp;

	FOnObjectDrop OnObjectDrop;

	FOnObjectPickUpSpawned OnObjectPickUpPreSpawned;

	FOnObjectPickUpSpawned OnObjectPickUpPostSpawned;

	FOnObjectDropSpawned OnObjectDropPreSpawned;

	FOnObjectDropSpawned OnObjectDropPostSpawned;

	void AttachEventHandlers( const bool bEnable , const EPickUp bPickUpOrDrop );

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBeginOverlap( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult );

	UFUNCTION()
	void OnPickUpCallback(TScriptInterface<IPickingUp> InCaller, const bool bCallPickUp);
	
	UFUNCTION()
	void OnDropCallback(TScriptInterface<IPickingUp> InCaller, const bool bCallDrop);

};
