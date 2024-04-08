// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerController.h"

#include "MyCharacter.h"
#include "MyInGameHUD.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	const auto& HUD = Cast<AMyInGameHUD>(GetHUD());
	const auto& Target = Cast<AMyCharacter>(GetCharacter());

	if (IsValid(HUD) && IsValid(Target))
	{
		HUD->BindPlayer(Target);
	}
}

void AMyPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

