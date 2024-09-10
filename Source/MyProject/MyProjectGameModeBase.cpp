// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyProjectGameModeBase.h"

#include "AIController.h"
#include "MyCharacter.h"
#include "MyCollectable.h"
#include "MyGameState.h"
#include "MyInGameHUD.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "MySpectatorPawn.h"
#include "MyStatComponent.h"
#include "MyWeapon.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"

#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

AMyProjectGameModeBase::AMyProjectGameModeBase()
	: TSpawnPoint(nullptr),
	  CTSpawnPoint(nullptr)
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
	SpectatorClass   = AMySpectatorPawn::StaticClass();
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

	PlayerState->OnStateChanged.AddUniqueDynamic(MyGameState, &AMyGameState::HandlePlayerStateChanged);
	PlayerState->SetState(EMyCharacterState::Alive);
	PlayerState->BindOnKillOccurred(MyGameState, &AMyGameState::HandleKillOccurred);
	MyGameState->HandleNewPlayer(PlayerState);

	if (!IsValid(MyGameState))
	{
		LOG_FUNC(LogTemp, Error, "GameState is not valid");
		return;
	}

	if (MyGameState->GetState() == EMyRoundProgress::Unknown &&
		 MyGameState->GetCTCount() > 0 &&
		 MyGameState->GetTCount() > 0)
	{
		LOG_FUNC(LogTemp, Warning, "Restart Round");
		MyGameState->Reset();
		MyGameState->RestartRound();
	}
}

AActor* AMyProjectGameModeBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	const auto& Start = PickPlayerStart(Player);

	return IsValid(Start) ? Start : Super::FindPlayerStart_Implementation(Player, IncomingName);
}

void AMyProjectGameModeBase::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	if (const auto& PlayerState = Cast<AMyPlayerState>(NewPlayer->PlayerState))
	{
		const auto& Character = Cast<AMyCharacter>(NewPlayer->GetPawn());

		if (IsValid(PlayerState))
		{
			PlayerState->OnHandChanged.AddUniqueDynamic(Character, &AMyCharacter::OnHandChanged);
		}

		if (const auto& Collectable = PlayerState->GetCurrentHand())
		{
			if (IsValid(Character))
			{
				if (const auto& Weapon = Cast<AMyWeapon>(Collectable))
				{
					PlayerState->SetCurrentWeapon(Weapon);
				}
				else
				{
					PlayerState->SetCurrentItem(Collectable);
				}
			}
		}
	}

}

void AMyProjectGameModeBase::InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer)
{
	Super::InitStartSpot_Implementation(StartSpot, NewPlayer);

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	for (const auto& PlayerStart : PlayerStarts)
	{
		const auto& Start = Cast<APlayerStart>(PlayerStart);

		if (IsValid(Start))
		{
			if (Start->PlayerStartTag == "T")
			{
				TSpawnPoint = Start;
			}
			else if (Start->PlayerStartTag == "CT")
			{
				CTSpawnPoint = Start;
			}
		}
	}
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

		if (Team == EMyTeam::CT)
		{
			return CTSpawnPoint;
		}
		else
		{
			return TSpawnPoint;
		}
	}

	return nullptr;
}

AActor* AMyProjectGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	const auto& Start = PickPlayerStart(Player);

	return IsValid(Start) ? Start : Super::ChoosePlayerStart_Implementation(Player);
}

