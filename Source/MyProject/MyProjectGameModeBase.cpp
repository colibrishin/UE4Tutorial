// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyProjectGameModeBase.h"

#include "MyCharacter.h"
#include "MyInGameHUD.h"
#include "MyPlayerController.h"

#include "GameFramework/GameStateBase.h"

#include "Kismet/GameplayStatics.h"

AMyProjectGameModeBase::AMyProjectGameModeBase()
{
	// StaticClass, 컴파일 타임 타입
	// GetClass, 런타임 타입 (Base class 포인터)
	// DefaultPawnClass = AMyCharacter::StaticClass();

	static ConstructorHelpers::FClassFinder<AMyCharacter> BP_Char
		(TEXT("Blueprint'/Game/Blueprints/BPMyCharacter.BPMyCharacter_C'"));

	if (BP_Char.Succeeded()) { DefaultPawnClass = BP_Char.Class; }

	static ConstructorHelpers::FClassFinder<AMyInGameHUD> BP_HUD(TEXT("Class'/Script/MyProject.MyInGameHUD'"));

	if (BP_HUD.Succeeded()) { HUDClass = BP_HUD.Class; }

	static ConstructorHelpers::FClassFinder<AMyPlayerController> BP_PC
		(TEXT("Blueprint'/Game/Blueprints/BPMyPlayerController.BPMyPlayerController_C'"));

	if (BP_PC.Succeeded()) { PlayerControllerClass = BP_PC.Class; }
}

void AMyProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// todo: not to start manually?
	StartMatch();
}

void AMyProjectGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasMatchStarted())
	{
		if (GetWorld()->GetGameState()->GetServerWorldTimeSeconds() >= MatchRoundTime)
		{
			EndMatch();
		}
	}
}

void AMyProjectGameModeBase::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	// Spawnpoints
}

void AMyProjectGameModeBase::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	// If Bomb has been exploded -> T win
	// If Bomb has been defused -> CT win
	// Ran out of time -> CT win
	// Eliminate all enemies -> T or CT win
}
