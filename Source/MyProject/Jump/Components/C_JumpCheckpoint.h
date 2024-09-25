// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"

#include "C_JumpCheckpoint.generated.h"


UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_JumpCheckpoint : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void MarkCheckpoint(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Sets default values for this component's properties
	UC_JumpCheckpoint();
	
	void UnbindDelegate();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;
	
	void BindDelegate();
	
	UPROPERTY(VisibleAnywhere)
	bool bMarkedPreviously;

public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction
	) override;
};
