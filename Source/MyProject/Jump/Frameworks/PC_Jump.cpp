// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_Jump.h"


// Sets default values
APC_Jump::APC_Jump()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APC_Jump::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APC_Jump::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

