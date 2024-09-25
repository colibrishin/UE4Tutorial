// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/Actors/BaseClass/A_Character.h"
#include "A_JumpCharacter.generated.h"

class UC_JumpRevert;

UCLASS()
class MYPROJECT_API AA_JumpCharacter : public AA_Character
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AA_JumpCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UC_JumpRevert* RevertComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
