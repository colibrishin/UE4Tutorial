// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_Jump.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
AGS_Jump::AGS_Jump()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AGS_Jump::ProcessWin(UC_PickUp* InPrevious, UC_PickUp* InNew)
{
	GetWorld()->GetTimerManager().SetTimer(
		WinDelay,
		this,
		&AGS_Jump::ChangeLevel,
		5.f,
		false);
}

void AGS_Jump::ChangeLevel() const
{
	UGameplayStatics::OpenLevel(GetWorld(), "Untitled", true);
}

// Called when the game starts or when spawned
void AGS_Jump::BeginPlay()
{
	Super::BeginPlay();

	OnCoinGained.AddUniqueDynamic(this, &AGS_Jump::ProcessWin);
}

// Called every frame
void AGS_Jump::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

