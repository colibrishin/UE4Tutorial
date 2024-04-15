// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyProjectGameModeBase.h"

#include "MyCharacter.h"
#include "MyGameState.h"
#include "MyInGameHUD.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"

#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

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

	GameStateClass = AMyGameState::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();
}

void AMyProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// todo: not to start manually?

	if (GetNumPlayers() == 2)
	{
		LOG_FUNC(LogTemp, Warning, "Starting match");
		StartMatch();
	}
}

void AMyProjectGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasMatchStarted())
	{
		if (GetWorld()->GetGameState()->GetServerWorldTimeSeconds() >= AMyGameState::MatchRoundTime)
		{
			EndMatch();
		}
	}
}

void AMyProjectGameModeBase::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void AMyProjectGameModeBase::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	// If Bomb has been exploded -> T win
	// If Bomb has been defused -> CT win
	// Ran out of time -> CT win
	// Eliminate all enemies -> T or CT win
}

AActor* AMyProjectGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	if (IsValid(Player))
	{
		const auto& PlayerState = Cast<AMyPlayerState>(Player->PlayerState);
		const auto& Team = PlayerState->GetTeam();

		TArray<AActor*> PlayerStarts;

		UGameplayStatics::GetAllActorsOfClass(this, TSubclassOf<APlayerStart>(), PlayerStarts);

		for (const auto& PlayerStart : PlayerStarts)
		{
			const auto& Start = Cast<APlayerStart>(PlayerStart);

			if (IsValid(Start))
			{
				if (Start->PlayerStartTag == *EnumToString(Team))
				{
					LOG_FUNC_PRINTF(LogTemp, Warning, "PlayerStartTag: %s", *Start->PlayerStartTag.ToString());
					return Start;
				}
			}
		}
	}


	return Super::ChoosePlayerStart_Implementation(Player);
}
