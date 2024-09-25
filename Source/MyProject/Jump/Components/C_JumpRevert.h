// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_JumpRevert.generated.h"


class UInputAction;
class UInputMappingContext;

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_JumpRevert : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_JumpRevert();

protected:
	UFUNCTION()
	void Revert();

	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* RevertAction;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime , ELevelTick TickType ,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
