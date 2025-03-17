// Fill out your copyright notice in the Description page of Project Settings.


#include "A_C4.h"

#include "MyProject/Components/C_Interactive.h"
#include "MyProject/Private/Enum.h"
#include "MyProject/Private/Utilities.hpp"
#include "Components/ShapeComponent.h"
#include "MyProject/Components/C_PickUp.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AA_C4::AA_C4(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUsePredicate = true;
	bDelay = true;

	InteractiveComponent = CreateDefaultSubobject<UC_Interactive>(TEXT("InteractiveComponent"));
	BombState = EMyBombState::Unknown;
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
		return PlantingTime / InteractiveComponent->GetElapsedTime();
	}

	return 0.f;
}

float AA_C4::GetElapsedDefuseTimeRatio() const
{
	if (BombState == EMyBombState::Defusing)
	{
		return DefusingTime / InteractiveComponent->GetElapsedTime();
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
		GetPickUpComponent()->OnObjectDropPreSpawned.AddUniqueDynamic(this, &AA_C4::HandlePlanted);
		GetPickUpComponent()->OnObjectPickUpSpawned.AddUniqueDynamic( this , &AA_C4::MutateCloned );
	}
}

bool AA_C4::PredicateInteraction()
{
	//todo: range check, viewport check

	return true;
}

void AA_C4::Interaction()
{
	if (BombState == EMyBombState::Planting)
	{
		Planter = InteractiveComponent->GetInteractor();
		SetState( EMyBombState::Planted );
		InteractiveComponent->SetDelayTime(DefusingTime);

		GetPickUpComponent()->OnObjectDrop.Broadcast( GetInteractiveComponent()->GetInteractor() , true );
		return;
	}
	else if (BombState == EMyBombState::Defusing)
	{
		Defuser = InteractiveComponent->GetInteractor();
		SetState( EMyBombState::Defused );
		InteractiveComponent->SetActive(false);
		return;
	}

	ensureAlwaysMsgf(false, TEXT("Uncaught state"));
}

void AA_C4::StartInteraction()
{
	switch ( BombState )
	{
	case EMyBombState::Planted:
		GetInteractiveComponent()->SetDelayTime( DefusingTime );
		SetState( EMyBombState::Defusing );
		Defuser = InteractiveComponent->GetInteractor();
		break;
	case EMyBombState::Idle:
		GetInteractiveComponent()->SetDelayTime( PlantingTime );
		SetState( EMyBombState::Planting );
		Planter = InteractiveComponent->GetInteractor();
		break;
	}
}

void AA_C4::StopInteraction()
{
	switch ( BombState )
	{
	case EMyBombState::Defusing:
		SetState( EMyBombState::Planted );
		Defuser = nullptr;
		break;
	case EMyBombState::Planting:
		SetState( EMyBombState::Idle );
		Planter = nullptr;
		break;
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
	DOREPLIFETIME(AA_C4, BombState);
	DOREPLIFETIME(AA_C4, Planter);
	DOREPLIFETIME(AA_C4, Defuser);
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
		MutateCloned( Cloned );

		if ( Cloned->BombState == EMyBombState::Planted ) 
		{
			// Mutates the bomb planter to the newly cloned object.
			Cloned->Planter = Planter;
			// Disable the pick up component to block the pick up the bomb by defuser.
			Cloned->GetPickUpComponent()->AttachEventHandlers( false , EPickUp::Drop );
			// Mutates the bomb state.
			Cloned->BombState = EMyBombState::Planted;
		}
	}
}

void AA_C4::MutateCloned( AActor* InSpawnedActor )
{
	if ( AA_C4* Cloned = Cast<AA_C4>( InSpawnedActor ) )
	{
		if ( !Cloned->IsDummy() )
		{
			Cloned->DetonationTime = DetonationTime;
			Cloned->DefusingTime = DefusingTime;
			Cloned->PlantingTime = PlantingTime;
		}
	}
}

void AA_C4::Tick( const float DeltaTime )
{
	if ( HasAuthority() && GetNetMode() != NM_Client && !IsDummy() ) 
	{
		if ( BombState == EMyBombState::Planted && 
			 AfterPlantElapsedTime == DetonationTime )
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

void AA_C4::OnRep_BombState(const EMyBombState InOldBombState) const
{
	if ( GetNetMode() == NM_Client ) 
	{
		OnBombStateChanged.Broadcast( InOldBombState , BombState , Planter , Defuser );
	}
}
