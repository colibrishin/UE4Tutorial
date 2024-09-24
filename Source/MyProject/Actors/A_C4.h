// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Interfaces/InteractiveObject.h"
#include "MyProject/Widgets/MyBombIndicatorWidget.h"

#include "A_C4.generated.h"

// Declare as non-dynamic delegate for forwarding to game state;
DECLARE_MULTICAST_DELEGATE_FourParams(FOnBombStateChanged, const EMyBombState, const EMyBombState, const AA_Character*, const AA_Character*);

enum class EMyBombState : uint8;
class UC_Interactive;
class AA_Character;

UCLASS()
class MYPROJECT_API AA_C4 : public AA_Collectable, public IInteractiveObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AA_C4(const FObjectInitializer& ObjectInitializer);

	FOnBombStateChanged OnBombStateChanged;

	UC_Interactive* GetInteractiveComponent() const { return InteractiveComponent; }

	float GetElapsedDetonationTime() const;

	float GetDetonationTime() const;
	
	float GetElapsedPlantTimeRatio() const;

	float GetElapsedDefuseTimeRatio() const;
	
	EMyBombState   GetBombState() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual bool PredicateInteraction() override;

	virtual void Interaction() override;

	virtual void StartInteraction() override;
	
	virtual void StopInteraction() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	void         OnRep_BombState(const EMyBombState InOldBombState) const;

private:
	UPROPERTY(EditAnywhere)
	float DetonationTime;

	UPROPERTY(EditAnywhere)
	float PlantingTime;

	UPROPERTY(EditAnywhere)
	float DefusingTime;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	UC_Interactive* InteractiveComponent;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_BombState)
	EMyBombState BombState;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	AA_Character* Planter;

	UPROPERTY(VisibleAnywhere, Replicated)
	AA_Character* Defuser;
	
};
