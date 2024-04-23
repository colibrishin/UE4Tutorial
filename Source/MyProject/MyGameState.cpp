// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyGameState.h"

#include "MyBombIndicatorWidget.h"
#include "MyC4.h"
#include "MyCharacter.h"
#include "MyCollectable.h"
#include "MyInGameHUD.h"
#include "MyInGameWidget.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "MyProjectGameModeBase.h"
#include "MySpectatorPawn.h"
#include "MyStatComponent.h"
#include "MyWeapon.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"

#include "Net/UnrealNetwork.h"

AMyGameState::AMyGameState()
	: RoundC4(nullptr),
	  RoundProgress(EMyRoundProgress::Unknown),
	  Winner(EMyTeam::Unknown),
	  CTWinCount(0),
	  TWinCount(0),
	  CTRoundWinSound(nullptr),
	  TRoundWinSound(nullptr),
	  bCanBuy(false),
	  BombState(EMyBombState::Unknown),
	  LastRoundInWorldTime(0),
	  AliveCT(0),
	  AliveT(0),
	  CurrentHandle(nullptr)
{
	static ConstructorHelpers::FObjectFinder<USoundWave> RoundStartSoundFinder
		(TEXT("SoundWave'/Game/Models/sounds/moveout.moveout'"));

	if (RoundStartSoundFinder.Succeeded())
	{
		RoundStartSound = RoundStartSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> CTRoundWinSoundFinder
		(
		 TEXT
		 (
		  "SoundWave'/Game/Models/sounds/counter-terrorist-win-cs-go-sound-effect-made-with-Voicemod.counter-terrorist-win-cs-go-sound-effect-made-with-Voicemod'"
		 )
		);

	if (CTRoundWinSoundFinder.Succeeded())
	{
		CTRoundWinSound = CTRoundWinSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> TRoundWinSoundFinder
		(
		 TEXT
		 (
		  "SoundWave'/Game/Models/sounds/terrorists-win-cs-go-sound-effect-made-with-Voicemod.terrorists-win-cs-go-sound-effect-made-with-Voicemod'"
		 )
		);

	if (TRoundWinSoundFinder.Succeeded())
	{
		TRoundWinSound = TRoundWinSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> BombPlantedSoundFinder (
		TEXT("SoundWave'/Game/Models/sounds/bomb-has-been-planted-sound-effect-cs-go-made-with-Voicemod.bomb-has-been-planted-sound-effect-cs-go-made-with-Voicemod'")
	);

	if (BombPlantedSoundFinder.Succeeded())
	{
		BombPlantedSound = BombPlantedSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> BombDefusedSoundFinder
	(
		TEXT("SoundWave'/Game/Models/sounds/bomb-has-been-defused-csgo-sound-effect.bomb-has-been-defused-csgo-sound-effect'")
	);

	if (BombDefusedSoundFinder.Succeeded())
	{
		BombDefusedSound = BombDefusedSoundFinder.Object;
	}

	static ConstructorHelpers::FClassFinder<AMyC4> BP_C4(TEXT("Blueprint'/Game/Blueprints/BPMyC4.BPMyC4_C'"));

	if (BP_C4.Succeeded()) { C4BluePrint = BP_C4.Class; }
}

void AMyGameState::HandleKillOccurred(AMyPlayerState* Killer, AMyPlayerState* Victim, const AMyWeapon* Weapon) const
{
	if (HasAuthority())
	{
		if (Killer && Victim)
		{
			Killer->IncrementKills();
			Victim->IncrementDeaths();

			Multi_KillOccurred(Killer, Victim, Weapon);
		}
	}
}

void AMyGameState::BuyTimeEnded()
{
	bCanBuy = false;

	if (HasAuthority())
	{
		OnBuyChanged.Broadcast(bCanBuy);
	}
}

void AMyGameState::HandleOnBombPicked(AMyCharacter* Character) const
{
	if (HasAuthority())
	{
		Multi_NotifyBombPicked(Character);
	}
}

void AMyGameState::OnRep_WinnerSet() const
{
	if (Winner != EMyTeam::Unknown)
	{
		UGameplayStatics::PlaySound2D(this, Winner == EMyTeam::CT ? CTRoundWinSound : TRoundWinSound);
	}

	OnWinnerSet.Broadcast(Winner);
}

void AMyGameState::OnRep_RoundProgress() const
{
	if (RoundProgress == EMyRoundProgress::Playing)
	{
		UGameplayStatics::PlaySound2D(this, RoundStartSound);
	}

	OnRoundProgressChanged.Broadcast(RoundProgress);
}

void AMyGameState::OnRep_CanBuy() const
{
	OnBuyChanged.Broadcast(bCanBuy);
}

void AMyGameState::OnRep_BombState(const EMyBombState PreviousBombState)
{
	if (PreviousBombState == EMyBombState::Planting && BombState == EMyBombState::Planted)
	{
		UGameplayStatics::PlaySound2D(this, BombPlantedSound);
	}

	if (PreviousBombState == EMyBombState::Defusing && BombState == EMyBombState::Defused)
	{
		UGameplayStatics::PlaySound2D(this, BombDefusedSound);
	}

	OnBombStateChanged(BombState);
}

void AMyGameState::OnRep_AliveCT() const
{
	OnAliveCountChanged.Broadcast(EMyTeam::CT, AliveCT);
}

void AMyGameState::OnRep_AliveT() const
{
	OnAliveCountChanged.Broadcast(EMyTeam::T, AliveT);
}

void AMyGameState::Multi_ResetBombIndicator_Implementation()
{
	if (const auto& HUD = GetWorld()->GetFirstPlayerController()->GetHUD())
	{
		if (const auto& InGameHUD = Cast<AMyInGameHUD>(HUD))
		{
			if (const auto& InGameWidget = InGameHUD->GetInGameWidget())
			{
				if (const auto& BombIndicator = InGameWidget->GetBombIndicatorWidget())
				{
					BombIndicator->Reset();
				}
			}
		}
	}
}

void AMyGameState::Multi_NotifyBombPicked_Implementation(AMyCharacter* Character) const
{
	OnBombPicked.Broadcast(Character);
}

void AMyGameState::Multi_NotifyNewPlayer_Implementation(AMyPlayerState* State) const
{
	OnNewPlayerJoined.Broadcast(State);
}

void AMyGameState::Multi_KillOccurred_Implementation(
	AMyPlayerState* Killer, AMyPlayerState* Victim, const AMyWeapon* Weapon
) const
{
	OnKillOccurred.Broadcast(Killer, Victim, Weapon);
}

void AMyGameState::HandlePlayerStateChanged(AMyPlayerController* PlayerController, const EMyTeam Team, const EMyCharacterState State)
{
	if (HasAuthority())
	{
		switch (State)
		{
		case EMyCharacterState::Alive:
			switch (Team)
			{
			case EMyTeam::CT: ++AliveCT; break;
			case EMyTeam::T: ++AliveT; break;
			case EMyTeam::Unknown:
			default: break;
			}
		case EMyCharacterState::Planting: 
			break;
		case EMyCharacterState::Defusing: 
			break;
		case EMyCharacterState::Dead:
			switch (Team)
			{
				case EMyTeam::CT: --AliveCT;
				break;
				case EMyTeam::T: --AliveT;
				break;
				case EMyTeam::Unknown: break;
			}
			break;
		case EMyCharacterState::Unknown:
		default: break;
		}

		if (State == EMyCharacterState::Alive || State == EMyCharacterState::Dead)
		{
			OnAliveCountChanged.Broadcast(Team, Team == EMyTeam::CT ? AliveCT : AliveT);
		}

		if (State == EMyCharacterState::Dead)
		{
			const auto& Character = Cast<AMyCharacter>(PlayerController->GetCharacter());

			const auto& Spectator = GetWorld()->SpawnActor<AMySpectatorPawn>(
				Character->GetActorLocation(),
				FRotator::ZeroRotator
			);

			if (Character)
			{
				if (const auto& Weapon = Character->GetWeapon())
				{
					Weapon->Drop();
				}
				if (const auto& Item = Character->GetCurrentItem())
				{
					Item->Drop();	
				}
			}

			Spectator->SetPreviousCharacter(Cast<AMyCharacter>(PlayerController->GetCharacter()));
			PlayerController->SetSpectator(Spectator);

			if (!PlayerController->HasAuthority())
			{
				PlayerController->Client_SetSpectator(Spectator);
			}
		}

		LOG_FUNC_PRINTF(LogTemp, Warning, "AliveCT: %d, AliveT: %d", AliveCT, AliveT);

		// Elimination
		if (BombState != EMyBombState::Planted)
		{
			if (RoundProgress == EMyRoundProgress::Playing && AliveCT <= 0)
			{
				SetWinner(EMyTeam::T);
				GoToRoundEnd();
			}
			else if (RoundProgress == EMyRoundProgress::Playing && AliveT <= 0)
			{
				SetWinner(EMyTeam::CT);
				GoToRoundEnd();
			}
		}

		OnPlayerStateChanged.Broadcast(PlayerController, Team, State);
	}
}

void AMyGameState::HandleNewPlayer(AMyPlayerState* State) const
{
	if (HasAuthority())
	{
		Multi_NotifyNewPlayer(State);
	}
}

void AMyGameState::HandleRoundProgress() const
{
	if (HasAuthority())
	{
		if (RoundProgress == EMyRoundProgress::FreezeTime)
		{
			LOG_FUNC(LogTemp, Warning, "FreezeTime");

			for (const auto& Player : PlayerArray)
			{
				const auto& PlayerController = Cast<AMyPlayerController>(Player->GetOwner());
				const auto& Character = Cast<AMyCharacter>(PlayerController->GetCharacter());

				if (!IsValid(Character))
				{
					continue;
				}

				LOG_FUNC(LogTemp, Warning, "SetMovementMode to None");
				Character->GetCharacterMovement()->SetMovementMode(MOVE_None);
			}
		}
		else if (RoundProgress == EMyRoundProgress::Playing)
		{
			UGameplayStatics::PlaySound2D(this, RoundStartSound);

			LOG_FUNC(LogTemp, Warning, "Playing");

			for (const auto& Player : PlayerArray)
			{
				const auto& PlayerController = Cast<AMyPlayerController>(Player->GetOwner());
				const auto& Character = Cast<AMyCharacter>(PlayerController->GetCharacter());

				if (!IsValid(Character))
				{
					continue;
				}

				LOG_FUNC(LogTemp, Warning, "SetMovementMode to Walking");
				Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}
		}
		else if (RoundProgress == EMyRoundProgress::PostRound)
		{
			
		}
	}
}

void AMyGameState::SetRoundProgress(const EMyRoundProgress NewProgress)
{
	if (HasAuthority())
	{
		RoundProgress = NewProgress;
		HandleRoundProgress();

		OnRoundProgressChanged.Broadcast(RoundProgress);
	}
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		Reset();
	}
}

void AMyGameState::Reset()
{
	Super::Reset();
	RoundProgress = EMyRoundProgress::Unknown;
	CTWinCount = 0;
	TWinCount = 0;
	Winner = EMyTeam::Unknown;
	bCanBuy = true;
	BombState = EMyBombState::Unknown;
	LastRoundInWorldTime = GetServerWorldTimeSeconds();
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyGameState, bCanBuy);
	DOREPLIFETIME(AMyGameState, RoundProgress);
	DOREPLIFETIME(AMyGameState, AliveCT);
	DOREPLIFETIME(AMyGameState, AliveT);
	DOREPLIFETIME(AMyGameState, LastRoundInWorldTime);
	DOREPLIFETIME(AMyGameState, Winner);
	DOREPLIFETIME(AMyGameState, CTWinCount);
	DOREPLIFETIME(AMyGameState, TWinCount);
	DOREPLIFETIME(AMyGameState, RoundC4);
	DOREPLIFETIME(AMyGameState, BombState);
}

void AMyGameState::OnBombStateChanged(const EMyBombState NewState)
{
	LOG_FUNC_PRINTF(LogTemp, Warning, "BombState: %s", *EnumToString(NewState));

	if (HasAuthority())
	{
		if (BombState == EMyBombState::Planting && NewState == EMyBombState::Planted)
		{
			UGameplayStatics::PlaySound2D(this, BombPlantedSound);
		}

		if (BombState == EMyBombState::Defusing && NewState == EMyBombState::Defused)
		{
			UGameplayStatics::PlaySound2D(this, BombDefusedSound);
		}

		BombState = NewState;
	}

	OnBombProgressChanged.Broadcast(BombState);

	if (BombState == EMyBombState::Defused)
	{
		SetWinner(EMyTeam::CT);
		GoToRoundEnd();
	}
	else if (BombState == EMyBombState::Exploded)
	{
		SetWinner(EMyTeam::T);
		GoToRoundEnd();
	}
}

void AMyGameState::RestartRound()
{
	if (!HasAuthority())
	{
		LOG_FUNC(LogTemp, Error, "This function should be called on server");
		return;
	}

	LOG_FUNC(LogTemp, Warning, "Restarting Round");
	SetWinner(EMyTeam::Unknown);

	AliveCT = 0;
	AliveT = 0;

	if (RoundC4)
	{
		if (OnBombStateChangedHandle.IsValid())
		{
			RoundC4->UnbindOnBombStateChanged(OnBombStateChangedHandle);
		}

		if (OnBombPickedHandle.IsValid())
		{
			RoundC4->UnbindOnBombPicked(OnBombPickedHandle);
		}
		
		RoundC4->Destroy(true);
	}

	BombState = EMyBombState::Unknown;

	Multi_ResetBombIndicator();

	for (const auto& Player : PlayerArray)
	{
		const auto& PlayerState = Cast<AMyPlayerState>(Player);
		const auto& PlayerController = Cast<AMyPlayerController>(Player->GetOwner());
		const auto& Character = Cast<AMyCharacter>(PlayerController->GetCharacter());

		PlayerState->Reset();

		if (IsValid(Character))
		{
			Character->Destroy(true);
		}
		else
		{
			const auto& Spectator = Cast<AMySpectatorPawn>(PlayerController->GetPawn());
			const auto& MainCharacter = Spectator->GetPreviousCharacter();

			if (MainCharacter.IsValid())
			{
				MainCharacter->Destroy(true);
			}

			if (IsValid(Spectator))
			{
				Spectator->Destroy(true);
			}
			

			if (PlayerController->HasAuthority())
			{
				PlayerController->SetSpectator(nullptr);
			}
			else
			{
				PlayerController->Client_SetSpectator(nullptr);
			}
		}

		if (const auto& GameMode = GetWorld()->GetAuthGameMode<AMyProjectGameModeBase>())
		{
			GameMode->RestartPlayer(PlayerController);
		}
	}

	// Cleanup collectables
	TArray<AActor*> Collectables;
	UGameplayStatics::GetAllActorsOfClass(this, AMyCollectable::StaticClass(), Collectables);

	for (const auto& Collectable : Collectables)
	{
		const auto& CastedCollectable = Cast<AMyCollectable>(Collectable);

		if (!CastedCollectable->GetItemOwner())
		{
			CastedCollectable->Destroy(true);
		}
	}

	const auto& TSpawnPoint = GetWorld()->GetAuthGameMode<AMyProjectGameModeBase>()->GetTSpawnPoint();
	const auto& SpawnPointLocation = TSpawnPoint->GetActorLocation();

	FActorSpawnParameters SpawnParameters;

	SpawnParameters.Owner = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld());

	const auto& C4 = GetWorld()->SpawnActor(
		C4BluePrint,
		&SpawnPointLocation,
		&FRotator::ZeroRotator,
		SpawnParameters
	);

	C4->SetReplicateMovement(true);
	C4->SetReplicates(true);

	RoundC4 = Cast<AMyC4>(C4);

	OnBombStateChangedHandle = RoundC4->BindOnBombStateChanged(this, &AMyGameState::OnBombStateChanged);
	BombState = EMyBombState::Idle;

	OnBombPickedHandle = RoundC4->BindOnBombPicked(this, &AMyGameState::HandleOnBombPicked);

	GetWorldTimerManager().ClearTimer(RoundTimerHandle);
	GetWorldTimerManager().ClearTimer(RoundEndTimerHandle);
	GetWorldTimerManager().ClearTimer(BuyTimeHandle);

	GetWorldTimerManager().SetTimer
	(
		BuyTimeHandle,
		this,
		&AMyGameState::BuyTimeEnded,
		MatchBuyTime,
		false
	);

	bCanBuy = true;

	GoToFreeze();
}

void AMyGameState::SetWinner(const EMyTeam NewWinner)
{
	if (HasAuthority())
	{
		Winner = NewWinner;

		if (Winner != EMyTeam::Unknown)
		{
			LOG_FUNC_PRINTF(LogTemp, Warning, "Winner: %s", *EnumToString(Winner));

			UGameplayStatics::PlaySound2D(this, Winner == EMyTeam::CT ? CTRoundWinSound : TRoundWinSound);
			(Winner == EMyTeam::CT ? CTWinCount : TWinCount)++;
			OnWinnerSet.Broadcast(Winner);
			GoToRoundEnd();
		}
	}
}

void AMyGameState::NextRound()
{
	if (GetState() == EMyRoundProgress::PostRound)
	{
		RecordRoundTime();
		RestartRound();
	}
}

void AMyGameState::OnRoundTimeRanOut()
{
	if (GetState() == EMyRoundProgress::Playing)
	{
		SetWinner(EMyTeam::CT);
		GoToRoundEnd();
	}
}

void AMyGameState::OnFreezeEnded()
{
	if (GetState() == EMyRoundProgress::FreezeTime)
	{
		GoToRound();
	}
}

void AMyGameState::TransitTo
(
	const EMyRoundProgress NextProgress,
	void(AMyGameState::* NextFunction)(), 
	const float Delay, 
	FTimerHandle& NextHandle
)
{
	if (CurrentHandle != nullptr)
	{
		GetWorldTimerManager().ClearTimer(*CurrentHandle);
	}

	GetWorldTimerManager().SetTimer
		(
		 NextHandle, 
		 this, 
		 NextFunction, 
		 Delay, 
		 false
		);

	SetRoundProgress(NextProgress);
	CurrentHandle = &NextHandle;
}

void AMyGameState::GoToFreeze()
{
	TransitTo
	(
		EMyRoundProgress::FreezeTime,
		&AMyGameState::OnFreezeEnded,
		AMyProjectGameModeBase::MatchFreezeTime,
		FreezeTimerHandle
	);
}

void AMyGameState::GoToRound()
{
	LOG_FUNC(LogTemp, Warning, "Round goes to playing");
	TransitTo
	(
		EMyRoundProgress::Playing,
		&AMyGameState::OnRoundTimeRanOut,
		AMyProjectGameModeBase::MatchRoundTime,
		RoundTimerHandle
	);
}

void AMyGameState::GoToRoundEnd()
{
	LOG_FUNC(LogTemp, Warning, "Round goes to post round");
	TransitTo
	(
		EMyRoundProgress::PostRound,
		&AMyGameState::NextRound,
		AMyProjectGameModeBase::MatchRoundEndDelay,
		RoundEndTimerHandle
	);
}
