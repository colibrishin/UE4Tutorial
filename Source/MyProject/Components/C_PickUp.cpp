// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PickUp.h"

#include "..\Interfaces\PickingUp.h"

#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Private/Utilities.hpp"

DEFINE_LOG_CATEGORY(LogPickUp);

// Sets default values for this component's properties
UC_PickUp::UC_PickUp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


void UC_PickUp::SetActive(bool bNewActive, bool bReset)
{
	Super::SetActive(bNewActive , bReset);

	if (!bNewActive)
	{
		OnObjectPickUp.RemoveAll(this);
		OnObjectDrop.RemoveAll(this);
		OnComponentBeginOverlap.RemoveAll(this);
	}
	else
	{
		AttachEventHandlers();
	}
}

void UC_PickUp::AttachEventHandlers()
{
	OnObjectPickUp.AddUniqueDynamic(this , &UC_PickUp::OnPickUpCallback);
	OnObjectDrop.AddUniqueDynamic(this , &UC_PickUp::OnDropCallback);
	OnComponentBeginOverlap.AddUniqueDynamic(this , &UC_PickUp::OnBeginOverlap);
}

// Called when the game starts
void UC_PickUp::BeginPlay()
{
	Super::BeginPlay();

	// Do not listen any event if parent object is dummy;
	if (const AA_Collectable* Collectable = Cast<AA_Collectable>(GetOwner());
		!Collectable->IsDummy())
	{
		AttachEventHandlers();
	}
}

void UC_PickUp::OnBeginOverlap(
	UPrimitiveComponent* /*OverlappedComponent*/ , AActor* OtherActor ,
	UPrimitiveComponent* /*OtherComp*/ , int32 /*OtherBodyIndex*/ , bool /*bFromSweep*/ ,
	const FHitResult& /*SweepResult*/
)
{
	if (Cast<IPickingUp>(OtherActor))
	{
		OnObjectPickUp.Broadcast(TScriptInterface<IPickingUp>(OtherActor), true);
	}
}

void UC_PickUp::OnPickUpCallback(TScriptInterface<IPickingUp> InCaller, const bool bCallPickUp)
{
	if (!InCaller)
	{
		LOG_FUNC(LogPickUp , Error , "Caught invalid pickup object!");
		return;
	}

	LOG_FUNC_PRINTF(LogPickUp , Log , "Caught pickup : %s" , *InCaller->_getUObject()->GetName());

	OnComponentBeginOverlap.RemoveAll(this);
	OnObjectPickUp.RemoveAll(this);
	OnObjectDrop.AddUniqueDynamic(this , &UC_PickUp::OnDropCallback);
	SetSimulatePhysics(false);

	if (bCallPickUp)
	{
		InCaller->PickUp(this);

		if (GetNetMode() != NM_Client)
		{
			// Assuming the object is cloned into child actor component or consumed etc;
			GetOwner()->Destroy(true);
		}
	}
}

void UC_PickUp::OnDropCallback(TScriptInterface<IPickingUp> InCaller, const bool bCallDrop)
{
	if (!InCaller)
	{
		LOG_FUNC(LogPickUp , Error , "Caught invalid drop object!");
		return;
	}

	LOG_FUNC_PRINTF(LogPickUp , Log , "Caught drop : %s" , *InCaller->_getUObject()->GetName());

	OnComponentBeginOverlap.AddUniqueDynamic(this , &UC_PickUp::OnBeginOverlap);
	OnObjectDrop.RemoveAll(this);
	OnObjectPickUp.AddUniqueDynamic(this , &UC_PickUp::OnPickUpCallback);
	SetSimulatePhysics(true);

	if (bCallDrop)
	{
		if (GetNetMode() != NM_Client)
		{
			// Clone the object before destroyed => ChildActorComponent->DestroyChildActor();
			AActor* Cloned = NewObject<AActor>(
				nullptr, GetOwner()->GetClass(), NAME_None, RF_NoFlags, GetOwner());

			Cloned->SetReplicates(true);
			Cloned->SetReplicateMovement(true);
			Cloned->SetOwner(GetWorld()->GetFirstPlayerController()); // Owning by server;

			FVector Origin, Extents;
			Cast<AActor>(InCaller.GetInterface())->GetActorBounds(true, Origin, Extents);

			const FVector RePickPrevention = Extents + GetOwner()->GetActorForwardVector();
			Cloned->SetActorLocation(GetOwner()->GetActorLocation() + RePickPrevention); // to avoid overlap;
			Cloned->GetComponentByClass<UMeshComponent>()->AddImpulse(Cloned->GetActorForwardVector() * 50.f); // Throw;
		}

		// Object destruction should be handled in InCaller's Drop;
		InCaller->Drop(this);
	}
}
