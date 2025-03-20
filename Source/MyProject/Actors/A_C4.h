// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Interfaces/InteractiveObject.h"

#include "A_C4.generated.h"

enum class EMyBombState : uint8;
class UC_Interactive;
class AA_Character;

USTRUCT( BlueprintType )
struct FBombStateContext
{
	GENERATED_BODY()
	
	UPROPERTY( VisibleAnywhere )
	EMyBombState OldBombState;
	
	UPROPERTY( VisibleAnywhere )
	EMyBombState NewBombState;
	
	UPROPERTY( VisibleAnywhere )
	AA_Character* Planter;

	UPROPERTY( VisibleAnywhere )
	AA_Character* Defuser;
};

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

	virtual bool PredicateInteraction( AA_Character* InInteractor ) const override;

	virtual void Interaction() override;

	UFUNCTION()
	virtual void HandlePrePlanted( AActor* InSpawnedActor );

	UFUNCTION()
	virtual void HandlePlanted( AActor* InSpawnedActor );

	UFUNCTION()
	void MutateCloned( AActor* InSpawnedActor );

	virtual void Tick( const float DeltaTime ) override;

	void SetState( const EMyBombState NewState );

	virtual void PostNetInit() override;

public:
	virtual bool StartClientInteraction( AA_Character* InInteractor ) const override;

	virtual bool StopClientInteraction() const override;
	
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

	UFUNCTION()
	virtual void OnBeginOverlap( AActor* OverlappedActor, AActor* OtherActor );
	
	UFUNCTION()
	virtual void OnEndOverlap( AActor* OverlappedActor , AActor* OtherActor );

	virtual void OnRep_CollisionComponent() override;

	bool IsAfterPlant() const;
	bool PredicateAfterPlant( AA_Character* InInteractor ) const;
	bool PredicateBeforePlant( AA_Character* InInteractor ) const;

private:
	UPROPERTY(EditAnywhere, Replicated)
	float DetonationTime;

	UPROPERTY(EditAnywhere, Replicated)
	float AfterPlantElapsedTime = 0.f;

	UPROPERTY(EditAnywhere, Replicated )
	float PlantingTime;

	UPROPERTY(EditAnywhere, Replicated )
	float DefusingTime;

	UPROPERTY(VisibleAnywhere)
	bool bOverlappingPlantableArea;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	UC_Interactive* InteractiveComponent;

	UPROPERTY(VisibleAnywhere, Replicated)
	AA_Character* Planter = nullptr;

	UPROPERTY(VisibleAnywhere, Replicated)
	AA_Character* Defuser = nullptr;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_BombState)
	EMyBombState BombState;
	
};
