// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Interactive.h"

#include "MyProject/Interfaces/InteractiveObject.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UC_Interactive::UC_Interactive()
{
	// enable and disable the tick only if the interaction timer and predication flag is set and triggered.
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.SetTickFunctionEnable(false);

	// ...
	static IInteractiveObject* ObjectTest = Cast<IInteractiveObject>(GetOwner());
	ensureAlwaysMsgf(ObjectTest, TEXT("Owner should be inherits IInteractiveObejct"));
}

void UC_Interactive::Interaction(AA_Character* InInteractor)
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	// If object is delayed interaction object, go for timer;
	// predication flag will determine whether interaction should be stopped if condition check is failed;
	if (bDelay)
	{
		ensure(!InteractionTimerHandle.IsValid());
		
		GetWorld()->GetTimerManager().SetTimer(
			InteractionTimerHandle,
			this,
			&UC_Interactive::HandleInteraction,
			DelayTime,
			false
		);

		if (bPredicating)
		{
			PrimaryComponentTick.SetTickFunctionEnable(true);
		}

		Interactor = InInteractor;
	}
	else
	{
		// if not, execute immediately;
		HandleInteraction();
	}
}

void UC_Interactive::StopInteraction()
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	// Interaction (delayed) -> Start Timer -> Canceled before timer;
	ResetTimerIfDelayed();
	
	IInteractiveObject* Object = Cast<IInteractiveObject>(GetOwner());
	ensure(Object);
	
	Object->StopInteraction();
	Interactor = nullptr;
}

// Called when the game starts
void UC_Interactive::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UC_Interactive::HandleInteraction()
{
	// Interaction (delayed) -> Start Timer -> Timer finished;
	ResetTimerIfDelayed();
	
	IInteractiveObject* Object = Cast<IInteractiveObject>(GetOwner());
	ensure(Object);
	
	Object->Interaction();
	Interactor = nullptr;
}

void UC_Interactive::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UC_Interactive, Interactor);
	DOREPLIFETIME(UC_Interactive, DelayTime);
}

// Called every frame
void UC_Interactive::TickComponent(
	float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	// tick will be ran if timer is triggered;
	if (InteractionTimerHandle.IsValid())
	{
		IInteractiveObject* Object = Cast<IInteractiveObject>(GetOwner());
		ensure(Object);

		if (!Object->PredicateInteraction())
		{
			StopInteraction();
		}
	}
}

