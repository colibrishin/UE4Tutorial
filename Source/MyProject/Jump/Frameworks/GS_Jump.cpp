// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_Jump.h"


// Sets default values
AGS_Jump::AGS_Jump()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGS_Jump::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGS_Jump::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

