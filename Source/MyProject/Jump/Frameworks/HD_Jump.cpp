// Fill out your copyright notice in the Description page of Project Settings.


#include "HD_Jump.h"


// Sets default values
AHD_Jump::AHD_Jump()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHD_Jump::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHD_Jump::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

