// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_Interactive.generated.h"


class AA_Character;
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
	
	void Interaction(AA_Character* InInteractor);

	void StopInteraction();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FORCEINLINE void ResetTimerIfDelayed()
	{
		if (bDelay)
		{
			ensure(InteractionTimerHandle.IsValid());
			GetWorld()->GetTimerManager().ClearTimer(InteractionTimerHandle);

			if (bPredicating)
			{
				PrimaryComponentTick.SetTickFunctionEnable(false);
			}
		}
	}
	
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
	AA_Character* Interactor;

	FTimerHandle InteractionTimerHandle;
	
public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction
	) override;
};
