// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Interactive.h"

#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Interfaces/InteractiveObject.h"
#include "MyProject/Private/Utilities.hpp"
#include "MyProject/MyGameState.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY( LogInteractiveComponent );

// Sets default values for this component's properties
UC_Interactive::UC_Interactive()
{
	// enable and disable the tick only if the interaction timer and predication flag is set and triggered.
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.SetTickFunctionEnable(false);
	SetIsReplicatedByDefault( true );

	// ...
	bInteracting = false;
}

bool UC_Interactive::ClientInteraction( AA_Character* InInteractor ) const
{
	if ( GetNetMode() != NM_Client )
	{
		return true;
	}

	if ( !IsActive() || bInteracting )
	{
		return false;
	}

	// Should be the first player controller
	check( GetWorld()->GetFirstPlayerController()->GetCharacter() == InInteractor );

	IInteractiveObject* Object = Cast<IInteractiveObject>( GetOwner() );
	check( Object );

	// Check if the predication is met.
	if ( bPredicating && !Object->PredicateInteraction( InInteractor ) )
	{
		LOG_FUNC( LogInteractiveComponent , Log , "Client side Initial predication failed. Won't do the interaction" );
		return false;
	}

	LOG_FUNC( LogInteractiveComponent , Log , "Client side starts Interaction." );
	LOG_FUNC_PRINTF( LogInteractiveComponent , Log , "Interactor: %s, Interactee: %s" , *InInteractor->GetName() , *GetOwner()->GetName() );

	return Object->StartClientInteraction( InInteractor );
}

bool UC_Interactive::StopClientInteraction() const
{
	if ( GetNetMode() != NM_Client )
	{
		return true;
	}

	if ( !IsActive() || !bInteracting )
	{
		return false;
	}

	IInteractiveObject* Object = Cast<IInteractiveObject>( GetOwner() );
	check( Object );
	return Object->StopClientInteraction();
}

void UC_Interactive::Interaction(AA_Character* InInteractor)
{
	if ( GetNetMode() == NM_Client || !IsActive() || bInteracting )
	{
		return;
	}

	// If object is delayed interaction object, go for timer;
	// predication flag will determine whether interaction should be stopped if condition check is failed;
	IInteractiveObject* Object = Cast<IInteractiveObject>( GetOwner() );
	check( Object );

	if ( bPredicating )
	{
		if ( !Object->PredicateInteraction( InInteractor ) )
		{
			LOG_FUNC( LogInteractiveComponent , Log , "Initial predication failed. Won't do the interaction" );
			return;
		}

		LOG_FUNC( LogInteractiveComponent , Log , "Tick-wise predicating is enabled" );
		PrimaryComponentTick.SetTickFunctionEnable( true );
	}
	
	if (bDelay)
	{
		ensure( !InteractionTimerHandle.IsValid() );

		LOG_FUNC_PRINTF( LogInteractiveComponent , Log , "Starts Interaction with timer. Expired after %f seconds" , DelayTime );

		GetWorld()->GetTimerManager().SetTimer(
			InteractionTimerHandle ,
			this ,
			&UC_Interactive::HandleInteraction ,
			DelayTime ,
			false
		);

		LOG_FUNC_PRINTF( LogInteractiveComponent , Log , "Interactor: %s, Interactee: %s" , *InInteractor->GetName() , *GetOwner()->GetName() );
		
		bInteracting = true;
		Interactor = InInteractor;
		InteractionStartWorldTime = GetWorld()->GetGameState<AMyGameState>()->GetServerWorldTimeSeconds();
		GetOwner()->SetOwner( InInteractor->GetController() );
		Object->StartInteraction();
	}
	else
	{
		// if not, execute immediately;
		bInteracting = true;
		Interactor = InInteractor;
		InteractionStartWorldTime = GetWorld()->GetGameState<AMyGameState>()->GetServerWorldTimeSeconds();
		GetOwner()->SetOwner( InInteractor->GetController() );
		HandleInteraction();
	}
}

void UC_Interactive::StopInteraction()
{
	if ( GetNetMode() == NM_Client || !IsActive() )
	{
		return;
	}

	// Interaction (delayed) -> Start Timer -> Canceled before timer;
	ResetTimerIfDelayed();
	
	IInteractiveObject* Object = Cast<IInteractiveObject>(GetOwner());
	ensure(Object);
	
	InteractionStartWorldTime = 0;
	Object->StopInteraction();

	GetOwner()->SetOwner( GetWorld()->GetFirstPlayerController() );
	bInteracting = false;
	Interactor = nullptr;
}

double UC_Interactive::GetInteractionStartTime() const
{
	check( bInteracting );
	return InteractionStartWorldTime;
}

// Called when the game starts
void UC_Interactive::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// Actor need to be ticked due to the tick-wise predicating.
	check( GetOwner()->PrimaryActorTick.bCanEverTick );

	TScriptInterface<IInteractiveObject> Owner = GetOwner();
	check( Owner );

	bPredicating = Owner->bUsePredicate;
	bDelay = Owner->bDelay;

	SetGenerateOverlapEvents( true );
	SetSimulatePhysics( false );
	SetEnableGravity( false );
	SetCollisionEnabled( ECollisionEnabled::QueryAndProbe );
}

inline void UC_Interactive::ResetTimerIfDelayed()
{
	if ( bDelay )
	{
		LOG_FUNC( LogInteractiveComponent , Log , "Delayed interaction canceled" );

		if ( InteractionTimerHandle.IsValid() )
		{
			GetWorld()->GetTimerManager().ClearTimer( InteractionTimerHandle );
		}

		if ( bPredicating )
		{
			LOG_FUNC( LogInteractiveComponent , Log , "Tick-wise predicating is disabled" );
			PrimaryComponentTick.SetTickFunctionEnable( false );
		}
	}
}

void UC_Interactive::HandleInteraction()
{
	// Interaction (delayed) -> Start Timer -> Timer finished;
	ResetTimerIfDelayed();
	
	IInteractiveObject* Object = Cast<IInteractiveObject>(GetOwner());
	ensure(Object);
	
	Object->Interaction();
	LOG_FUNC( LogInteractiveComponent , Log , "Clear the cached interactor" );
	Interactor = nullptr;
}

void UC_Interactive::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( UC_Interactive , bInteracting );
	DOREPLIFETIME( UC_Interactive , Interactor );
	DOREPLIFETIME( UC_Interactive , DelayTime );
	DOREPLIFETIME_CONDITION( UC_Interactive , InteractionStartWorldTime , COND_OwnerOnly );
}

// Called every frame
void UC_Interactive::TickComponent(
	float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	// tick will be ran if timer is triggered;
	if ( InteractionTimerHandle.IsValid() )
	{
		IInteractiveObject* Object = Cast<IInteractiveObject>(GetOwner());
		ensure(Object);

		if ( !Object->PredicateInteraction( GetInteractor() ) )
		{
			StopInteraction();
		}
	}
}

