// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Interfaces/InteractiveObject.h"

#include "C_Interactive.generated.h"

DECLARE_LOG_CATEGORY_EXTERN( LogInteractiveComponent , All , Log );

class AMyPlayerState;

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_Interactive : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_Interactive();

	AA_Character* GetInteractor() const { return Interactor; }

	float GetElapsedTime() const
	{
		return GetWorld()->GetTimerManager().GetTimerElapsed(InteractionTimerHandle);
	}

	void SetDelayTime(const float InTime) { DelayTime = InTime; }

	void SetDelayed( const bool InFlag ) { bDelay = InFlag; }

	float GetDelayedTime() const { return DelayTime; }

	bool CanInteract() const { return !bInteracting; }
	
	void Interaction(AA_Character* InInteractor);

	void StopInteraction();

	double GetInteractionStartTime() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ResetTimerIfDelayed();
	
	UFUNCTION()
	void HandleInteraction();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere)
	bool bDelay;

	UPROPERTY(VisibleAnywhere)
	bool bPredicating;

	UPROPERTY(VisibleAnywhere, Replicated)
	float DelayTime;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bInteracting;

	UPROPERTY(VisibleAnywhere, Replicated)
	double InteractionStartWorldTime;

	UPROPERTY(VisibleAnywhere, Replicated)
	AA_Character* Interactor;

	FTimerHandle InteractionTimerHandle;
	
public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction
	) override;
};
