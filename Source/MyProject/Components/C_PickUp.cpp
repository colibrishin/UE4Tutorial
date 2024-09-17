// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PickUp.h"

#include "..\Interfaces\PickingUp.h"
#include "MyProject/Private/Utilities.hpp"

DEFINE_LOG_CATEGORY(LogPickUp);

// Sets default values for this component's properties
UC_PickUp::UC_PickUp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	OnObjectPickUp.AddUniqueDynamic(this , &UC_PickUp::OnPickUpCallback);
	OnObjectDrop.AddUniqueDynamic(this , &UC_PickUp::OnDropCallback);
}


// Called when the game starts
void UC_PickUp::BeginPlay()
{
	Super::BeginPlay();

	// ...

	OnComponentBeginOverlap.AddUniqueDynamic(this , &UC_PickUp::OnBeginOverlap);
}

void UC_PickUp::OnBeginOverlap(
	UPrimitiveComponent* /*OverlappedComponent*/ , AActor* OtherActor ,
	UPrimitiveComponent* /*OtherComp*/ , int32 /*OtherBodyIndex*/ , bool /*bFromSweep*/ ,
	const FHitResult& /*SweepResult*/
)
{
	if (Cast<IPickingUp>(OtherActor))
	{
		OnObjectPickUp.Broadcast(TScriptInterface<IPickingUp>(OtherActor));
	}
}

void UC_PickUp::OnPickUpCallback(TScriptInterface<IPickingUp> InCaller)
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

	InCaller->PickUp(this);
}

void UC_PickUp::OnDropCallback(TScriptInterface<IPickingUp> InCaller)
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

	InCaller->Drop(this);
}
