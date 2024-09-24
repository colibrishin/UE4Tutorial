// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyProjectGameModeBase.h"

#include "MyGameState.h"
#include "MyInGameHUD.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "MySpectatorPawn.h"

#include "Actors/BaseClass/A_Character.h"
#include "Actors/BaseClass/A_Weapon.h"

#include "Components/C_Health.h"
#include "Components/Asset/C_CharacterAsset.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

AMyProjectGameModeBase::AMyProjectGameModeBase()
	: TSpawnPoint(nullptr),
	  CTSpawnPoint(nullptr)
{
	// StaticClass, 컴파일 타임 타입
	// GetClass, 런타임 타입 (Base class 포인터)
	// DefaultPawnClass = AMyCharacter::StaticClass();
	
	DefaultPawnClass = AA_Character::StaticClass();

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

	PlayerState->GetHealthComponent()->Reset();
	PlayerState->AddMoney(18000);

	const auto& MyGameState = GetGameState<AMyGameState>();

	if (!IsValid(MyGameState))
	{
		LOG_FUNC(LogTemp, Error, "GameState is not valid");
		return;
	}

	PlayerState->OnStateChanged.AddUniqueDynamic(MyGameState, &AMyGameState::HandlePlayerStateChanged);
	PlayerState->SetState(EMyCharacterState::Alive);
	PlayerState->OnKillOccurred.AddUniqueDynamic(MyGameState, &AMyGameState::HandleKillOccurred);
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

APawn* AMyProjectGameModeBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	{
		// Server Context;
		const FTransform Transform {FQuat::Identity, StartSpot->GetActorLocation(), FVector::OneVector};

		auto* DefaultCharacter = GetWorld()->SpawnActorDeferred<AA_Character>(
			AA_Character::StaticClass(),
			Transform,
			NewPlayer,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
			ESpawnActorScaleMethod::OverrideRootScale);

		DefaultCharacter->GetAssetComponent()->SetID(NewPlayer->GetPlayerState<AMyPlayerState>()->GetCharacterAssetID());
		DefaultCharacter->FetchAsset<UC_CharacterAsset>();
		UGameplayStatics::FinishSpawningActor(DefaultCharacter, Transform, ESpawnActorScaleMethod::OverrideRootScale);

		return DefaultCharacter;
	}
}

AActor* AMyProjectGameModeBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	const auto& Start = PickPlayerStart(Player);

	return IsValid(Start) ? Start : Super::FindPlayerStart_Implementation(Player, IncomingName);
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

