// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyProjectGameModeBase.h"

#include "CommonRoundProgress.hpp"
#include "MyCharacter.h"
#include "MyGameState.h"
#include "MyInGameHUD.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "MyStatComponent.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"

#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

AMyProjectGameModeBase::AMyProjectGameModeBase()
	: CurrentHandle(nullptr)
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

	GameStateClass   = AMyGameState::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();
}

void AMyProjectGameModeBase::NextRound()
{
	const auto& MyGameState = GetGameState<AMyGameState>();

	if (MyGameState->GetState() == EMyRoundProgress::PostRound)
	{
		TArray<AActor*> Players;
		UGameplayStatics::GetAllActorsOfClass(this, AMyCharacter::StaticClass(), Players);

		for (const auto& Player : Players)
		{
			const auto& MyController = Cast<AMyPlayerController>(
				Player->GetNetOwningPlayer()->GetPlayerController(GetWorld()));

			if (IsValid(MyController))
			{
				RestartPlayer(MyController);
			}
		}

		GoToFreeze();
	}
}

void AMyProjectGameModeBase::OnRoundTimeRanOut()
{
	const auto& MyGameState = GetGameState<AMyGameState>();

	if (MyGameState->GetState() == EMyRoundProgress::Playing)
	{
		GoToRoundEnd();
	}
}

void AMyProjectGameModeBase::OnFreezeEnded()
{
	const auto& MyGameState = GetGameState<AMyGameState>();

	if (MyGameState->GetState() == EMyRoundProgress::FreezeTime)
	{
		GoToRound();
	}
}

void AMyProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GoToFreeze();
}

void AMyProjectGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMyProjectGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const auto& PlayerState = NewPlayer->GetPlayerState<AMyPlayerState>();

	if (!IsValid(PlayerState))
	{
		LOG_FUNC(LogTemp, Error, "PlayerState is not valid");
		return;
	}

	PlayerState->SetHP(PlayerState->GetStatComponent()->GetMaxHealth());
	PlayerState->AddMoney(18000);

	const auto& MyGameState = GetGameState<AMyGameState>();

	if (!IsValid(MyGameState))
	{
		LOG_FUNC(LogTemp, Error, "GameState is not valid");
		return;
	}

	HandleRoundProgressChanged(Cast<AMyCharacter>(NewPlayer->GetCharacter()), MyGameState->GetState());
}

AActor* AMyProjectGameModeBase::PickPlayerStart(AController* Player) const
{
	if (IsValid(Player))
	{
		const auto& PlayerState = Cast<AMyPlayerState>(Player->PlayerState);
		auto        Team        = PlayerState->GetTeam();

		if (HasAuthority() && Team == EMyTeam::Unknown)
		{
			PlayerState->AssignTeam();
			Team = PlayerState->GetTeam();
		}

		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

		LOG_FUNC_PRINTF(LogTemp, Warning, "PlayerStarts Count: %d", PlayerStarts.Num());
		LOG_FUNC_PRINTF(LogTemp, Warning, "Player Team: %s", *EnumToString(Team));

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

	return nullptr;
}

AActor* AMyProjectGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	const auto& Start = PickPlayerStart(Player);

	return IsValid(Start) ? Start : Super::ChoosePlayerStart_Implementation(Player);
}

void AMyProjectGameModeBase::TransitTo(
	const EMyRoundProgress NextProgress, const TMulticastDelegate<void()>& NextDelegate,
	void(AMyProjectGameModeBase::* NextFunction)(), const float Delay, FTimerHandle& NextHandle
)
{
	if (CurrentHandle != nullptr)
	{
		GetWorldTimerManager().ClearTimer(*CurrentHandle);
	}

	NextDelegate.Broadcast();

	GetWorldTimerManager().SetTimer
		(
		 NextHandle, 
		 this, 
		 NextFunction, 
		 Delay, 
		 false
		);

	GetGameState<AMyGameState>()->SetRoundProgress(NextProgress);
	CurrentHandle = &NextHandle;
}

void AMyProjectGameModeBase::GoToFreeze()
{
	LOG_FUNC(LogTemp, Warning, "Round goes to freeze");
	TransitTo
	(
		EMyRoundProgress::FreezeTime,
		OnFreezeStarted,
		&AMyProjectGameModeBase::OnFreezeEnded,
		MatchFreezeTime,
		FreezeTimerHandle
	);
}

void AMyProjectGameModeBase::GoToRound()
{
	LOG_FUNC(LogTemp, Warning, "Round goes to playing");
	TransitTo
	(
		EMyRoundProgress::Playing,
		OnRoundStarted,
		&AMyProjectGameModeBase::OnRoundTimeRanOut,
		MatchRoundTime,
		RoundTimerHandle
	);
}

void AMyProjectGameModeBase::GoToRoundEnd()
{
	LOG_FUNC(LogTemp, Warning, "Round goes to post round");
	TransitTo
	(
		EMyRoundProgress::PostRound,
		OnRoundEnded,
		&AMyProjectGameModeBase::NextRound,
		MatchRoundEndDelay,
		RoundEndTimerHandle
	);
}
