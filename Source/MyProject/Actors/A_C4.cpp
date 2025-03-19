// Fill out your copyright notice in the Description page of Project Settings.


#include "A_C4.h"

#include "MyProject/Components/C_Interactive.h"
#include "MyProject/Private/Enum.h"
#include "MyProject/Private/Utilities.hpp"
#include "Components/ShapeComponent.h"
#include "MyProject/Components/C_PickUp.h"
#include "MyProject/MyPlayerState.h"
#include "MyProject/MyGameState.h"
#include "MyProject/MyPlayerController.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AA_C4::AA_C4(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUsePredicate = true;
	bDelay = true;

	InteractiveComponent = CreateDefaultSubobject<UC_Interactive>(TEXT("InteractiveComponent"));
	BombState = EMyBombState::Idle;
}

float AA_C4::GetAfterPlantElapsedTime() const
{
	return AfterPlantElapsedTime;
}

float AA_C4::GetDetonationTime() const
{
	return DetonationTime;
}

float AA_C4::GetElapsedPlantTimeRatio() const
{
	if (BombState == EMyBombState::Planting)
	{
		return ( GetWorld()->GetGameState<AMyGameState>()->GetServerWorldTimeSeconds() - InteractiveComponent->GetInteractionStartTime() ) / PlantingTime;
	}

	return 0.f;
}

float AA_C4::GetElapsedDefuseTimeRatio() const
{
	if (BombState == EMyBombState::Defusing)
	{
		return ( GetWorld()->GetGameState<AMyGameState>()->GetServerWorldTimeSeconds() - InteractiveComponent->GetInteractionStartTime() ) / DefusingTime;
	}

	return 0.f;
}

EMyBombState AA_C4::GetBombState() const
{
	return BombState;
}

// Called when the game starts or when spawned
void  AA_C4::BeginPlay()
{
	Super::BeginPlay();
	
	if ( HasAuthority() )
	{
		BombState = EMyBombState::Idle;
		GetInteractiveComponent()->SetActive( true );
		GetInteractiveComponent()->SetDelayTime( PlantingTime );
		GetPickUpComponent()->OnObjectDropPreSpawned.AddUniqueDynamic( this , &AA_C4::MutateCloned );
		GetPickUpComponent()->OnObjectDropPostSpawned.AddUniqueDynamic( this, &AA_C4::HandlePlanted );
		GetPickUpComponent()->OnObjectPickUpPostSpawned.AddUniqueDynamic( this , &AA_C4::MutateCloned );
	}
}

bool AA_C4::PredicateInteraction()
{
	// todo: range check, viewport check
	if ( ( BombState == EMyBombState::Planted || BombState == EMyBombState::Defusing ) &&
		 InteractiveComponent->GetInteractor()->GetPlayerState<AMyPlayerState>()->GetTeam() != EMyTeam::CT )
	{
		return false;
	}
	
	if ( ( BombState == EMyBombState::Idle || BombState == EMyBombState::Planting ) &&
		 InteractiveComponent->GetInteractor()->GetPlayerState<AMyPlayerState>()->GetTeam() != EMyTeam::T )
	{
		return false;
	}

	return true;
}

void AA_C4::Interaction()
{
	switch ( BombState )
	{
	case EMyBombState::Planting:
		if ( InteractiveComponent->GetInteractor()->GetPlayerState<AMyPlayerState>()->GetTeam() != EMyTeam::T )
		{
			break;
		}
		Planter = InteractiveComponent->GetInteractor();
		SetState( EMyBombState::Planted );

		GetPickUpComponent()->OnObjectDrop.Broadcast( GetInteractiveComponent()->GetInteractor() , true );
		break;

	case EMyBombState::Defusing:
		if ( InteractiveComponent->GetInteractor()->GetPlayerState<AMyPlayerState>()->GetTeam() != EMyTeam::CT )
		{
			break;
		}
		Defuser = InteractiveComponent->GetInteractor();
		SetState( EMyBombState::Defused );

		// No longer interactable.
		InteractiveComponent->SetActive(false);
		break;
	case EMyBombState::Idle:
		break;
	case EMyBombState::Planted:
		break;
	case EMyBombState::Defused:
		break;
	case EMyBombState::Exploded:
		break;
	default: ;
	}
}

void AA_C4::HandlePrePlanted( AActor* InSpawnedActor )
{
	if ( AA_C4* Cloned = Cast<AA_C4>( InSpawnedActor ) )
	{
		MutateCloned( Cloned );
	}
}

void AA_C4::StartInteraction()
{
	switch ( BombState )
	{
	case EMyBombState::Planted:
		if ( InteractiveComponent->GetInteractor()->GetPlayerState<AMyPlayerState>()->GetTeam() != EMyTeam::CT )
		{
			break;
		}

		GetInteractiveComponent()->SetDelayTime( DefusingTime );
		SetState( EMyBombState::Defusing );
		Defuser = InteractiveComponent->GetInteractor();
		break;
	case EMyBombState::Idle:
		if ( InteractiveComponent->GetInteractor()->GetPlayerState<AMyPlayerState>()->GetTeam() != EMyTeam::T )
		{
			break;
		}

		// Block the case where the character does not own the bomb
		if ( GetParentActor() != InteractiveComponent->GetInteractor() )
		{
			break;
		}
		GetInteractiveComponent()->SetDelayTime( PlantingTime );
		SetState( EMyBombState::Planting );
		Planter = InteractiveComponent->GetInteractor();
		break;
	case EMyBombState::Planting:
		break;
	case EMyBombState::Defusing:
		break;
	case EMyBombState::Defused:
		break;
	case EMyBombState::Exploded:
		break;
	default: ;
	}
}

void AA_C4::StopInteraction()
{
	switch ( BombState )
	{
	case EMyBombState::Defusing:
		Defuser = nullptr;
		SetState( EMyBombState::Planted );
		break;
	case EMyBombState::Planting:
		Planter = nullptr;
		SetState( EMyBombState::Idle );
		break;
	case EMyBombState::Idle:
		break;
	case EMyBombState::Planted:
		break;
	case EMyBombState::Defused:
		break;
	case EMyBombState::Exploded:
		break;
	default: ;
	}
}

void AA_C4::SetDetonationTime( const float InTime )
{
	if ( HasAuthority() ) 
	{
		DetonationTime = InTime;
	}
}

void AA_C4::SetPlantingTime( const float InTime )
{
	if ( HasAuthority() )
	{
		PlantingTime = InTime;
	}
}

void AA_C4::SetDefusingTime( const float InTime )
{
	if ( HasAuthority() )
	{
		DefusingTime = InTime;
	}
}

void AA_C4::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( AA_C4 , DetonationTime );
	DOREPLIFETIME( AA_C4 , AfterPlantElapsedTime );
	DOREPLIFETIME( AA_C4 , PlantingTime );
	DOREPLIFETIME( AA_C4 , DefusingTime );
	DOREPLIFETIME(AA_C4, InteractiveComponent);
	DOREPLIFETIME(AA_C4, Planter);
	DOREPLIFETIME(AA_C4, Defuser);
	DOREPLIFETIME(AA_C4, BombState);
}

void AA_C4::PostFetchAsset()
{
	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionProfileName("MyC4");
	}
}

void AA_C4::HandlePlanted(AActor* InSpawnedActor)
{
	if (AA_C4* Cloned = Cast<AA_C4>(InSpawnedActor))
	{
		if ( BombState == EMyBombState::Planted && !Cloned->IsDummy() ) 
		{
			// Mutates the bomb planter to the newly cloned object.
			Cloned->Planter = Planter;
			// Disable the pick up component to block the pick up the bomb by defuser.
			Cloned->GetPickUpComponent()->AttachEventHandlers( false , EPickUp::Drop );
			Cloned->SetState( BombState );
		}
	}
}

void AA_C4::MutateCloned( AActor* InSpawnedActor )
{
	if ( AA_C4* Cloned = Cast<AA_C4>( InSpawnedActor ) )
	{
		if ( !Cloned->IsDummy() )
		{
			// Mutates the bomb state.
			Cloned->DetonationTime = DetonationTime;
			Cloned->DefusingTime = DefusingTime;
			Cloned->PlantingTime = PlantingTime;
			Cloned->BombState = BombState;

			// Preemptive setup for the delay timer
			switch ( Cloned->BombState )
			{
			case EMyBombState::Idle:
				Cloned->GetInteractiveComponent()->SetDelayTime( PlantingTime );
				break;
			case EMyBombState::Planted:
				Cloned->GetInteractiveComponent()->SetDelayTime( DefusingTime );
				break;
			}
		}
	}
}

void AA_C4::Tick( const float DeltaTime )
{
	if ( HasAuthority() && GetNetMode() != NM_Client && !IsDummy() ) 
	{
		if ( BombState == EMyBombState::Planted && 
			 AfterPlantElapsedTime >= DetonationTime )
		{
			SetState( EMyBombState::Exploded );
			PrimaryActorTick.SetTickFunctionEnable( false );
		}

		if ( BombState == EMyBombState::Planted )
		{
			AfterPlantElapsedTime += DeltaTime;
		}
	}	
}

void AA_C4::SetState( const EMyBombState NewState )
{
	const EMyBombState OldBombState = BombState;
	LOG_FUNC_PRINTF( LogTemp , Log , "C4 State changes %s -> %s" , *EnumToString( OldBombState ) , *EnumToString( NewState ) );
	BombState = NewState;
	OnBombStateChanged.Broadcast( OldBombState , NewState , Planter , Defuser );
}

void AA_C4::OnRep_BombState(const EMyBombState InOldBombState)
{
	if ( GetNetMode() == NM_Client ) 
	{
		// Since the actor replication occurred first before the GameState cache,
		// the bomb state changed delegation might have not bound.
		// Requesting the RPC to make sure that server and client sees the same bomb.
		if ( const AMyGameState* MyGameState = GetWorld()->GetGameState<AMyGameState>();
			 MyGameState && !OnBombStateChanged.IsBoundToObject( &MyGameState->OnBombStateChanged ) )
		{
			LOG_FUNC( LogTemp , Warning , "Bomb has been replicated before the game state, Update the delegation" );
			GetWorld()->GetFirstPlayerController<AMyPlayerController>()->Server_ValidateUpdateRebroadcastC4( this , { InOldBombState, BombState, Planter, Defuser } );
			return;
		}

		LOG_FUNC_PRINTF( LogTemp , Log , "Replicating C4 State %s -> %s" , *EnumToString( InOldBombState ) , *EnumToString( BombState ) );
		OnBombStateChanged.Broadcast( InOldBombState , BombState , Planter , Defuser );
	}
}
