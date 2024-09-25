// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_JumpFloorMovement.generated.h"


UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_JumpFloorMovement : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class UC_JumpMovingFloorAsset;
	
	// Sets default values for this component's properties
	UC_JumpFloorMovement();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Duration;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bRotating;

	FTimerHandle MoveTimerHandle;

public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction
	) override;
};
