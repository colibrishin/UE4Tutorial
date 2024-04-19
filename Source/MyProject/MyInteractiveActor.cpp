// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInteractiveActor.h"

// Sets default values
AMyInteractiveActor::AMyInteractiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AMyInteractiveActor::InteractImpl(AMyCharacter* Character)
{
	LOG_FUNC(LogTemp, Warning, "InteractImpl");

	if (HasAuthority())
	{
		ClientInteractImpl(Character);
	}
}

void AMyInteractiveActor::ClientInteractImpl(AMyCharacter* Character)
{
	LOG_FUNC(LogTemp, Warning, "ClientInteractImpl");
}

void AMyInteractiveActor::Client_Interact_Implementation(AMyCharacter* Character)
{
	ClientInteractImpl(Character);
}

void AMyInteractiveActor::Interact(AMyCharacter* Character)
{
	ExecuteServer(
	              this,
	              &AMyInteractiveActor::Server_Interact,
	              &AMyInteractiveActor::InteractImpl,
	              Character
	             );
}

void AMyInteractiveActor::Server_Interact_Implementation(AMyCharacter* Character)
{
	InteractImpl(Character);
	LOG_FUNC(LogTemp, Warning, "Do Client side call");
	Client_Interact(Character);
}

void AMyInteractiveActor::UseImpl(AMyCharacter* Character)
{
	LOG_FUNC(LogTemp, Warning, "UseImpl");
}

void AMyInteractiveActor::Use(AMyCharacter* Character)
{
	ExecuteServer(
	              this,
	              &AMyInteractiveActor::Server_Use,
	              &AMyInteractiveActor::UseImpl,
	              Character
	             );
}

void AMyInteractiveActor::Server_Use_Implementation(AMyCharacter* Character)
{
	UseImpl(Character);
}

void AMyInteractiveActor::InteractInterruptedImpl()
{
	LOG_FUNC(LogTemp, Warning, "InteractInterruptedImpl");
}

void AMyInteractiveActor::InteractInterrupted()
{
	ExecuteServer(
	              this,
	              &AMyInteractiveActor::Server_InteractInterrupted,
	              &AMyInteractiveActor::InteractInterruptedImpl
	             );
}

void AMyInteractiveActor::Server_InteractInterrupted_Implementation()
{
	InteractInterruptedImpl();
}

void AMyInteractiveActor::UseInterrupted()
{
	ExecuteServer(
	              this,
	              &AMyInteractiveActor::Server_UseInterrupted,
	              &AMyInteractiveActor::UseInterruptedImpl
	             );
}

void AMyInteractiveActor::Server_UseInterrupted_Implementation()
{
	UseInterruptedImpl();
}

void AMyInteractiveActor::UseInterruptedImpl()
{
	LOG_FUNC(LogTemp, Warning, "UseInterruptedImpl");
}