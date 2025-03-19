// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Interfaces/InteractiveObject.h"

#include "A_C4.generated.h"

enum class EMyBombState : uint8;
class UC_Interactive;
class AA_Character;

// Declare as non-dynamic delegate for forwarding to game state;
DECLARE_MULTICAST_DELEGATE_FourParams(FOnBombStateChanged, const EMyBombState, const EMyBombState, const AA_Character*, const AA_Character*);

UCLASS()
class MYPROJECT_API AA_C4 : public AA_Collectable, public IInteractiveObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AA_C4(const FObjectInitializer& ObjectInitializer);

	FOnBombStateChanged OnBombStateChanged;

	UC_Interactive* GetInteractiveComponent() const { return InteractiveComponent; }

	float GetAfterPlantElapsedTime() const;

	float GetDetonationTime() const;
	
	float GetElapsedPlantTimeRatio() const;

	float GetElapsedDefuseTimeRatio() const;
	
	EMyBombState   GetBombState() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool PredicateInteraction() override;

	virtual void Interaction() override;

	UFUNCTION()
	virtual void HandlePrePlanted( AActor* InSpawnedActor );

	UFUNCTION()
	virtual void HandlePlanted( AActor* InSpawnedActor );

	UFUNCTION()
	void MutateCloned( AActor* InSpawnedActor );

	virtual void Tick( const float DeltaTime ) override;

	void SetState( const EMyBombState NewState );

public:
	virtual void StartInteraction() override;
	
	virtual void StopInteraction() override;

	void SetDetonationTime(const float InTime);
	void SetPlantingTime( const float InTime );
	void SetDefusingTime( const float InTime );

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostFetchAsset() override;

protected:
	UFUNCTION()
	void OnRep_BombState(const EMyBombState InOldBombState);

private:
	UPROPERTY(EditAnywhere, Replicated)
	float DetonationTime;

	UPROPERTY(EditAnywhere, Replicated)
	float AfterPlantElapsedTime = 0.f;

	UPROPERTY(EditAnywhere, Replicated )
	float PlantingTime;

	UPROPERTY(EditAnywhere, Replicated )
	float DefusingTime;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	UC_Interactive* InteractiveComponent;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_BombState)
	EMyBombState BombState;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	AA_Character* Planter = nullptr;

	UPROPERTY(VisibleAnywhere, Replicated)
	AA_Character* Defuser = nullptr;
	
};
