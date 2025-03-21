// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/Interfaces/InteractiveObject.h"
#include "Components/SphereComponent.h"

#include "MyProject/Interfaces/ShapeAdjust.h"

#include "C_Interactive.generated.h"

DECLARE_LOG_CATEGORY_EXTERN( LogInteractiveComponent , All , Log );

class AMyPlayerState;

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_Interactive : public USphereComponent, public IShapeAdjust
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

	[[nodiscard]] float GetDelayedTime() const { return DelayTime; }

	[[nodiscard]] bool CanInteract() const { return !bInteracting; }

	[[nodiscard]] bool ClientInteraction( AA_Character* InInteractor ) const;

	[[nodiscard]] bool StopClientInteraction() const;
	
	void Interaction(AA_Character* InInteractor);

	void StopInteraction();

	[[nodiscard]] double GetInteractionStartTime() const;

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
	float InteractionStartWorldTime;

	UPROPERTY(VisibleAnywhere, Replicated)
	AA_Character* Interactor;

	FTimerHandle InteractionTimerHandle;
	
public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction
	) override;
};
