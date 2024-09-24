// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/Actors/MyBuyZone.h"

#include "MyProject/Private/Enum.h"

// Sets default values
AMyBuyZone::AMyBuyZone()
	: Team(EMyTeam::Unknown)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyBuyZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyBuyZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

