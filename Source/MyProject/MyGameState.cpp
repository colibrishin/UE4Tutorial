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
#include "Components/ShapeComponent.h"

#include "Kismet/GameplayStatics.h"

#include "MyProject/Widgets/MyBombIndicatorWidget.h"
#include "MyProject/Widgets/MyInGameWidget.h"
#include "Net/UnrealNetwork.h"
#include "MyProject/Components/Asset/C_CollectableAsset.h"
#include "MyProject/Components/C_Interactive.h"

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
	PrimaryActorTick.bCanEverTick = true;

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
	MatchRoundTime = 240.f;
}

void AMyGameState::HandleKillOccurred(AMyPlayerController* Killer, AMyPlayerController* Victim, UC_PickUp* Weapon)
{
	if (Killer && Victim)
	{
		AMyPlayerState* KillerPlayerState = Killer->GetPlayerState<AMyPlayerState>();
		AMyPlayerState* VictimPlayerState = Victim->GetPlayerState<AMyPlayerState>();

		check( KillerPlayerState && VictimPlayerState );

		KillerPlayerState->IncrementKills();
		VictimPlayerState->IncrementDeaths();
	}
}

void AMyGameState::BuyTimeEnded()
{
	bCanBuy = false;
	OnBuyChanged.Broadcast(bCanBuy);
}

void AMyGameState::HandleOnBombPicked( AActor* InC4Actor )
{
	if ( AA_C4* Bomb = Cast<AA_C4>( InC4Actor ) )
	{
		// Assuming that bomb is the child actor of the character
		Multi_NotifyBombPicked( Cast<AA_Character>( Bomb->GetParentActor() ) );
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

void AMyGameState::OnRep_AliveCT() const
{
	OnAliveCountChanged.Broadcast(EMyTeam::CT, AliveCT);
}

void AMyGameState::OnRep_AliveT() const
{
	OnAliveCountChanged.Broadcast(EMyTeam::T, AliveT);
}

void AMyGameState::Multi_NotifyBombPicked_Implementation(AA_Character* Character) const
{
	// todo: broadcast to the picker and the teams, not to the everyone.
	OnBombPicked.Broadcast(Character);
}

void AMyGameState::Multi_NotifyNewPlayer_Implementation(AMyPlayerState* State) const
{
	OnNewPlayerJoined.Broadcast(State);
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
	if (RoundC4)
	{
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
	}

	OnPlayerStateChanged.Broadcast(PlayerState, State);
}

void AMyGameState::HandleNewPlayer(AMyPlayerState* State) const
{
	Multi_NotifyNewPlayer(State);
}

void AMyGameState::HandleRoundProgress() const
{
	switch ( RoundProgress )
	{
	case EMyRoundProgress::FreezeTime:
		LOG_FUNC( LogTemp , Warning , "FreezeTime" );

		for ( const auto& Player : PlayerArray )
		{
			const auto& PlayerController = Cast<AMyPlayerController>( Player->GetOwner() );
			const auto& Character = Cast<AA_Character>( PlayerController->GetCharacter() );

			if ( !IsValid( Character ) )
			{
				continue;
			}

			LOG_FUNC( LogTemp , Warning , "SetMovementMode to None" );
			Character->GetCharacterMovement()->SetMovementMode( MOVE_None );
		}
		break;
	case EMyRoundProgress::Playing:
		UGameplayStatics::PlaySound2D( this , RoundStartSound );
		LOG_FUNC( LogTemp , Warning , "Playing" );

		for ( const auto& Player : PlayerArray )
		{
			const auto& PlayerController = Cast<AMyPlayerController>( Player->GetOwner() );
			const auto& Character = Cast<AA_Character>( PlayerController->GetCharacter() );

			if ( !IsValid( Character ) )
			{
				continue;
			}

			LOG_FUNC( LogTemp , Warning , "SetMovementMode to Walking" );
			Character->GetCharacterMovement()->SetMovementMode( MOVE_Walking );
		}
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
	DOREPLIFETIME( AMyGameState , StartTickInServerTime );
	DOREPLIFETIME(AMyGameState, Winner);
	DOREPLIFETIME(AMyGameState, CTWinCount);
	DOREPLIFETIME(AMyGameState, TWinCount);
	DOREPLIFETIME(AMyGameState, RoundC4);
}

void AMyGameState::Tick( const float DeltaTime )
{
	Super::Tick( DeltaTime );

	if ( HasAuthority() )
	{
		StartTickInServerTime = GetServerWorldTimeSeconds();
	}
}

void AMyGameState::UpdateC4( AActor* InNewActor )
{
	if ( AA_C4* InNewC4 = Cast<AA_C4>( InNewActor );
		 InNewC4 && !InNewC4->IsDummy() )
	{
		if ( RoundC4 )
		{
			RoundC4->OnBombStateChanged.Remove( C4DelegateHandle );
		}

		if ( HasAuthority() )
		{
			RoundC4 = InNewC4;	
		}

		C4DelegateHandle = InNewC4->OnBombStateChanged.AddRaw( &OnBombStateChanged , &FOnBombStateChangedDynamic::Broadcast );

		if ( UC_PickUp* PickUpComponent = InNewC4->GetPickUpComponent() )
		{
			PickUpComponent->OnObjectDropPreSpawned.AddUniqueDynamic( this , &AMyGameState::UpdateC4 );
			PickUpComponent->OnObjectPickUpPreSpawned.AddUniqueDynamic( this , &AMyGameState::UpdateC4 );
			PickUpComponent->OnObjectPickUpPostSpawned.AddUniqueDynamic( this , &AMyGameState::HandleOnBombPicked );
		}
	}
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

	if ( InNewState == EMyBombState::Defused )
	{
		SetWinner( EMyTeam::CT );
		GoToRoundEnd();
	}
	else if ( InNewState == EMyBombState::Exploded )
	{
		SetWinner( EMyTeam::T );
		GoToRoundEnd();
	}
}

void AMyGameState::RestartRound()
{
	if ( !HasAuthority() )
	{
		return;
	}

	LOG_FUNC(LogTemp, Warning, "Restarting Round");
	SetWinner(EMyTeam::Unknown);

	AliveCT = 0;
	AliveT = 0;

	if ( RoundC4 && C4DelegateHandle.IsValid() )
	{
		RoundC4->OnBombStateChanged.Remove( C4DelegateHandle );
		RoundC4->Destroy(true);
	}
	
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
			if (AMySpectatorPawn* Spectator = Cast<AMySpectatorPawn>(PlayerController->GetPawn()))
			{
				const auto& MainCharacter = Spectator->GetPreviousCharacter();

				if (MainCharacter.IsValid())
				{
					MainCharacter->Destroy(true);
				}

				if (IsValid(Spectator))
				{
					Spectator->Destroy(true);
				}
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
	const auto& SpawnPointLocation = TSpawnPoint->GetActorLocation() + FVector::UpVector * 25.f;

	FTransform SpawnTransform( FRotator::ZeroRotator , SpawnPointLocation , FVector::OneVector );
	AA_C4* const& C4 = GetWorld()->SpawnActorDeferred<AA_C4>(
		AA_C4::StaticClass(),
		FTransform::Identity,
		GetWorld()->GetFirstPlayerController(),
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	C4->SetReplicateMovement(true);
	C4->SetReplicates( true );
	C4->GetAssetComponent()->SetID( 5 );

	UGameplayStatics::FinishSpawningActor( C4 , SpawnTransform );

	// Server-side collision only
	C4->GetCollisionComponent()->SetSimulatePhysics( true );
	C4->GetCollisionComponent()->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );

	C4->SetPhysicsInClient( false );
	C4->SetCollisionTypeInClient( ECollisionEnabled::QueryAndProbe );

	C4->SetDetonationTime( 35.f );
	C4->SetDefusingTime( 10.f );
	C4->SetPlantingTime( 5.f );

	if ( UC_PickUp* PickUpComponent = C4->GetPickUpComponent() )
	{
		PickUpComponent->AttachEventHandlers( true , EPickUp::PickUp );
	}

	UpdateC4( C4 );

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
	LOG_FUNC_PRINTF( LogTemp , Warning , "Winner: %s" , *EnumToString( NewWinner ) );
	
	if ( NewWinner != EMyTeam::Unknown )
	{
		UGameplayStatics::PlaySound2D( this , NewWinner == EMyTeam::CT ? CTRoundWinSound : TRoundWinSound );

		if ( HasAuthority() )
		{
			Winner = NewWinner;
			( Winner == EMyTeam::CT ? CTWinCount : TWinCount )++;
			GoToRoundEnd();
		}

		OnWinnerSet.Broadcast( Winner );
	}
}

void AMyGameState::OnRep_C4( AA_C4* PreviousC4 )
{
	if ( GetNetMode() == NM_Client )
	{
		if ( C4DelegateHandle.IsValid() )
		{
			if ( PreviousC4 )
			{
				PreviousC4->OnBombStateChanged.Remove( C4DelegateHandle );
			}

			C4DelegateHandle.Reset();
		}
		if ( RoundC4 )
		{
			C4DelegateHandle = RoundC4->OnBombStateChanged.AddRaw( &OnBombStateChanged , &FOnBombStateChangedDynamic::Broadcast );
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
	if ( GetState() == EMyRoundProgress::Playing)
	{
		SetWinner(EMyTeam::CT);
		GoToRoundEnd();
	}
}

void AMyGameState::OnFreezeEnded()
{
	if ( GetState() == EMyRoundProgress::FreezeTime)
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
