// Fill out your copyright notice in the Description page of Project Settings.


#include "A_JumpCharacter.h"

#include "GameFramework/PawnMovementComponent.h"
#include "MyProject/Jump/Components/C_JumpRevert.h"


// Sets default values
AA_JumpCharacter::AA_JumpCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RevertComponent = CreateDefaultSubobject<UC_JumpRevert>(TEXT("RevertComponent"));
}

// Called when the game starts or when spawned
void AA_JumpCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AA_JumpCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AA_JumpCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

