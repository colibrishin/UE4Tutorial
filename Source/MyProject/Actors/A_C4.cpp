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
#include "MyProject/Actors/MyPlantableArea.h"
#include "MyProject/Actors/BaseClass/A_Weapon.h"
#include "MyProject/Components/Weapon/C_Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AA_C4::AA_C4(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUsePredicate = true;
	bDelay = true;

	InteractiveComponent = CreateDefaultSubobject<UC_Interactive>(TEXT("InteractiveComponent"));
	InteractiveComponent->SetIsReplicated( true );
	InteractiveComponent->SetNetAddressable();

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

	OnActorBeginOverlap.AddUniqueDynamic( this , &AA_C4::OnBeginOverlap );
	OnActorEndOverlap.AddUniqueDynamic( this , &AA_C4::OnEndOverlap );
}

bool AA_C4::PredicateInteraction( AA_Character* InInteractor )
{
	return IsAfterPlant() ? PredicateAfterPlant( InInteractor  ) : PredicateBeforePlant( InInteractor );
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
		Planter->GetController<AMyPlayerController>()->UnfreezePlayer();
		SetState( EMyBombState::Planted );

		GetPickUpComponent()->OnObjectDrop.Broadcast( GetInteractiveComponent()->GetInteractor() , true );
		break;

	case EMyBombState::Defusing:
		if ( InteractiveComponent->GetInteractor()->GetPlayerState<AMyPlayerState>()->GetTeam() != EMyTeam::CT )
		{
			break;
		}
		Defuser->GetController<AMyPlayerController>()->UnfreezePlayer();
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
		Defuser = InteractiveComponent->GetInteractor();
		Defuser->GetController<AMyPlayerController>()->FreezePlayer();
		SetState( EMyBombState::Defusing );
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
		Planter = InteractiveComponent->GetInteractor();
		Planter->GetController<AMyPlayerController>()->FreezePlayer();
		SetState( EMyBombState::Planting );
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
		InteractiveComponent->GetInteractor()->GetController<AMyPlayerController>()->UnfreezePlayer();
		Defuser = nullptr;
		SetState( EMyBombState::Planted );
		break;
	case EMyBombState::Planting:
		InteractiveComponent->GetInteractor()->GetController<AMyPlayerController>()->UnfreezePlayer();
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
	Super::PostFetchAsset();

	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionProfileName("MyC4");
	}

	if ( InteractiveComponent && CollisionComponent )
	{
		// Since the collision component is disabled in the client side, the GetActorBounds returns
		// zero. Sets the interaction range from the collision component bounds.
		FBoxSphereBounds Bounds = CollisionComponent->GetLocalBounds();
		InteractiveComponent->AttachToComponent( SkeletalMeshComponent , FAttachmentTransformRules::SnapToTargetNotIncludingScale );
		InteractiveComponent->SetCollisionProfileName( "MyC4" );
		InteractiveComponent->SetSphereRadius( Bounds.SphereRadius * 1.5f );
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

bool AA_C4::StartClientInteraction( AA_Character* InInteractor ) const
{
	InInteractor->GetCharacterMovement()->StopMovementImmediately();
	InInteractor->GetCharacterMovement()->SetMovementMode( MOVE_None );
	return IsAfterPlant() ? PredicateAfterPlant( InInteractor ) : PredicateBeforePlant( InInteractor );

	check( false );
	return false;
}

bool AA_C4::StopClientInteraction() const
{
	if ( AA_Character* Character = GetInteractiveComponent()->GetInteractor() )
	{
		Character->GetCharacterMovement()->SetMovementMode( MOVE_Walking );
		return IsAfterPlant() ? PredicateAfterPlant( Character ) : PredicateBeforePlant( Character );
	}

	check( false );
	return false;
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

void AA_C4::OnBeginOverlap( AActor* OverlappedActor , AActor* OtherActor )
{
	if ( Cast<AMyPlantableArea>( OtherActor ) ) 
	{
		bOverlappingPlantableArea = true;
	}
}

void AA_C4::OnEndOverlap( AActor* OverlappedActor , AActor* OtherActor )
{
	if ( Cast<AMyPlantableArea>( OtherActor ) )
	{
		bOverlappingPlantableArea = false;
	}
}

void AA_C4::OnRep_CollisionComponent()
{
	Super::OnRep_CollisionComponent();

	if ( CollisionComponent && InteractiveComponent )
	{
		FBoxSphereBounds Bounds = CollisionComponent->GetLocalBounds();
		InteractiveComponent->SetSphereRadius( Bounds.SphereRadius * 1.5f );
	}
}

bool AA_C4::IsAfterPlant() const
{
	return BombState == EMyBombState::Planted || BombState == EMyBombState::Defusing;
}

bool AA_C4::PredicateAfterPlant( AA_Character* InInteractor ) const
{
	// Counter terrorist can defuse the bomb
	if ( ( BombState == EMyBombState::Planted || BombState == EMyBombState::Defusing ) &&
		 InInteractor->GetPlayerState<AMyPlayerState>()->GetTeam() != EMyTeam::CT )
	{
		LOG_FUNC( LogTemp , Log , "C4 defusing is allowed to only CT" );
		return false;
	}

	// Cannot defuse the bomb while firing the weapon
	if ( AA_Weapon* WeaponActor = Cast<AA_Weapon>( InInteractor->GetHand()->GetChildActor() );
		 WeaponActor && WeaponActor->GetWeaponComponent()->IsFiring() )
	{
		LOG_FUNC( LogTemp , Log , "C4 cannot be defused while firing" );
		return false;
	}

	// Bomb should be in the player screen
	if ( FVector2D ScreenPosition;
		 !Cast<AMyPlayerController>( InInteractor->GetController() )->ProjectWorldLocationToScreen(GetActorLocation(), ScreenPosition ) )
	{
		LOG_FUNC( LogTemp , Log , "Out of the player viewport" );
		return false;
	}

	// Player should stay still while planting the bomb
	if ( InInteractor->GetVelocity() != FVector::ZeroVector )
	{
		LOG_FUNC( LogTemp , Log , "Player need to stop" );
		return false;
	}

	return true;
}

bool AA_C4::PredicateBeforePlant( AA_Character* InInteractor ) const
{
	// Terrorist can plant the bomb
	if ( ( BombState == EMyBombState::Idle || BombState == EMyBombState::Planting ) &&
		 InInteractor->GetPlayerState<AMyPlayerState>()->GetTeam() != EMyTeam::T )
	{
		LOG_FUNC( LogTemp , Warning , "C4 Planting is allowed to only T" );
		return false;
	}

	// If C4 is not in the plantable area, it cannot be planted
	if ( !bOverlappingPlantableArea )
	{
		LOG_FUNC( LogTemp , Warning , "Location is not a plantable area" );
		return false;
	}

	// C4 need to be equipped to be planted
	if ( InInteractor->GetHand()->GetChildActor() != this )
	{
		LOG_FUNC( LogTemp , Warning , "C4 does not equiped" );
		return false;
	}

	// Player should stay still while planting the bomb
	if ( InInteractor->GetVelocity() != FVector::ZeroVector )
	{
		LOG_FUNC( LogTemp , Warning , "Player need to stop" );
		return false;
	}

	return true;
}
