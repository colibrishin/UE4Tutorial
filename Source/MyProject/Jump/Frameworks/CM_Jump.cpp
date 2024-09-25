// Fill out your copyright notice in the Description page of Project Settings.


#include "CM_Jump.h"


// Sets default values
ACM_Jump::ACM_Jump()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultFOV = 90.0f;

	ViewPitchMin = -40.f;
	ViewPitchMax = 20.0f;

	ViewYawMin = 0;
	ViewYawMax = 359.99f;
}

// Called when the game starts or when spawned
void ACM_Jump::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACM_Jump::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

