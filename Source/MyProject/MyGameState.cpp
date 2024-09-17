// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyGameState.h"

#include "MyC4.h"
#include "MyInGameHUD.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "MyProjectGameModeBase.h"
#include "MySpectatorPawn.h"

#include "Actors/A_C4.h"
#include "Actors/BaseClass/A_Character.h"
#include "Actors/BaseClass/A_Collectable.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "MyProject/Widgets/MyBombIndicatorWidget.h"
#include "MyProject/Widgets/MyInGameWidget.h"
#include "Net/UnrealNetwork.h"

AMyGameState::AMyGameState()
	: CTRoundWinSound(nullptr),
	  TRoundWinSound(nullptr),
	  RoundProgress(EMyRoundProgress::Unknown),
	  Winner(EMyTeam::Unknown),
	  CTWinCount(0),
	  TWinCount(0),
	  bCanBuy(false),
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

	OnBombStateChanged.AddUniqueDynamic(this, &AMyGameState::HandleBombStateChanged);
}

void AMyGameState::HandleKillOccurred(AMyPlayerState* Killer, AMyPlayerState* Victim, UC_PickUp* Weapon)
{
	if (Killer && Victim)
	{
		Killer->IncrementKills();
		Victim->IncrementDeaths();

		Multi_KillOccurred(Killer, Victim, Weapon);
	}
}

void AMyGameState::BuyTimeEnded()
{
	bCanBuy = false;
	OnBuyChanged.Broadcast(bCanBuy);
}

void AMyGameState::HandleOnBombPicked(AA_Character* Character) const
{
	Multi_NotifyBombPicked(Character);
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

void AMyGameState::Multi_NotifyBombPicked_Implementation(AA_Character* Character) const
{
	OnBombPicked.Broadcast(Character);
}

void AMyGameState::Multi_NotifyNewPlayer_Implementation(AMyPlayerState* State) const
{
	OnNewPlayerJoined.Broadcast(State);
}

void AMyGameState::Multi_KillOccurred_Implementation(
	AMyPlayerState* Killer, AMyPlayerState* Victim, UC_PickUp* Weapon
) const
{
	OnKillOccurred.Broadcast(Killer, Victim, Weapon);
}

void AMyGameState::HandlePlayerStateChanged(AMyPlayerState* PlayerState, const EMyCharacterState State)
{
	const auto& Team = PlayerState->GetTeam();

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
		const auto& Character = Cast<AA_Character>(PlayerState->GetPawn());

		const auto& Spectator = GetWorld()->SpawnActor<AMySpectatorPawn>(
			Character->GetActorLocation(),
			FRotator::ZeroRotator
		);

		const auto& PlayerController = Cast<AMyPlayerController>(PlayerState->GetOwner());

		Spectator->SetPreviousCharacter(Character);
		PlayerController->SetSpectator(Spectator);

		if (!PlayerController->HasAuthority())
		{
			PlayerController->Client_SetSpectator(Spectator);
		}
	}

	LOG_FUNC_PRINTF(LogTemp, Warning, "AliveCT: %d, AliveT: %d", AliveCT, AliveT);

	// Elimination
	if (RoundC4->GetBombState() != EMyBombState::Planted)
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

	OnPlayerStateChanged.Broadcast(PlayerState, State);
}

void AMyGameState::HandleNewPlayer(AMyPlayerState* State) const
{
	Multi_NotifyNewPlayer(State);
}

void AMyGameState::HandleRoundProgress() const
{
	if (RoundProgress == EMyRoundProgress::FreezeTime)
	{
		LOG_FUNC(LogTemp, Warning, "FreezeTime");

		for (const auto& Player : PlayerArray)
		{
			const auto& PlayerController = Cast<AMyPlayerController>(Player->GetOwner());
			const auto& Character = Cast<AA_Character>(PlayerController->GetCharacter());

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
			const auto& Character = Cast<AA_Character>(PlayerController->GetCharacter());

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

void AMyGameState::SetRoundProgress(const EMyRoundProgress NewProgress)
{
	RoundProgress = NewProgress;
	HandleRoundProgress();

	OnRoundProgressChanged.Broadcast(RoundProgress);
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();

	Reset();
}

void AMyGameState::Reset()
{
	Super::Reset();
	RoundProgress = EMyRoundProgress::Unknown;
	CTWinCount = 0;
	TWinCount = 0;
	Winner = EMyTeam::Unknown;
	bCanBuy = true;
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
}

void AMyGameState::HandleBombStateChanged(const EMyBombState InOldState, const EMyBombState InNewState, const AA_Character* InPlanter, const AA_Character* InDefuser)
{
	LOG_FUNC_PRINTF(LogTemp, Warning, "BombState: %s", *EnumToString(InNewState));
	
	if (InOldState == EMyBombState::Planting && InNewState == EMyBombState::Planted)
	{
		UGameplayStatics::PlaySound2D(this, BombPlantedSound);
	}

	if (InOldState == EMyBombState::Defusing && InNewState == EMyBombState::Defused)
	{
		UGameplayStatics::PlaySound2D(this, BombDefusedSound);
	}

	if (InNewState == EMyBombState::Defused)
	{
		SetWinner(EMyTeam::CT);
		GoToRoundEnd();
	}
	else if (InNewState == EMyBombState::Exploded)
	{
		SetWinner(EMyTeam::T);
		GoToRoundEnd();
	}
}

void AMyGameState::RestartRound()
{
	LOG_FUNC(LogTemp, Warning, "Restarting Round");
	SetWinner(EMyTeam::Unknown);

	AliveCT = 0;
	AliveT = 0;

	if (RoundC4)
	{
		RoundC4->OnBombStateChanged.RemoveAll(this);
		RoundC4->Destroy(true);
	}
	
	Multi_ResetBombIndicator();

	for (const auto& Player : PlayerArray)
	{
		const auto& PlayerState = Cast<AMyPlayerState>(Player);
		const auto& PlayerController = Cast<AMyPlayerController>(Player->GetOwner());
		const auto& Character = Cast<AA_Character>(PlayerController->GetCharacter());

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
	UGameplayStatics::GetAllActorsOfClass(this, AA_Collectable::StaticClass(), Collectables);

	for (const auto& Collectable : Collectables)
	{
		const auto& CastedCollectable = Cast<AA_Collectable>(Collectable);

		if (!CastedCollectable->GetAttachParentActor())
		{
			CastedCollectable->Destroy(true);
		}
	}

	const auto& TSpawnPoint = GetWorld()->GetAuthGameMode<AMyProjectGameModeBase>()->GetTSpawnPoint();
	const auto& SpawnPointLocation = TSpawnPoint->GetActorLocation();

	FActorSpawnParameters SpawnParameters;

	SpawnParameters.Owner = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld());

	AA_C4* const& C4 = GetWorld()->SpawnActor<AA_C4>(
		AA_C4::StaticClass(),
		SpawnPointLocation,
		FRotator::ZeroRotator,
		SpawnParameters
	);

	C4->SetReplicateMovement(true);
	C4->SetReplicates(true);

	RoundC4 = Cast<AA_C4>(C4);
	RoundC4->OnBombStateChanged.AddRaw(&OnBombStateChanged, &FOnBombStateChangedDynamic::Broadcast);

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
