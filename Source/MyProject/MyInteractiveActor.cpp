// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInteractiveActor.h"

// Sets default values
AMyInteractiveActor::AMyInteractiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AMyInteractiveActor::Multi_Interact_Implementation(AMyCharacter* Character)
{
}

void AMyInteractiveActor::Multi_Use_Implementation(AMyCharacter* Character)
{
}

void AMyInteractiveActor::Multi_InteractInterrupted_Implementation()
{
}

void AMyInteractiveActor::Multi_UseInterrupted_Implementation()
{
}

void AMyInteractiveActor::Server_Interact_Implementation(AMyCharacter* Character)
{
}

void AMyInteractiveActor::Client_Interact_Implementation(AMyCharacter* Character)
{
}

void AMyInteractiveActor::Server_Use_Implementation(AMyCharacter* Character)
{
}

void AMyInteractiveActor::Client_Use_Implementation(AMyCharacter* Character)
{
}

void AMyInteractiveActor::Server_InteractInterrupted_Implementation()
{
}

void AMyInteractiveActor::Client_InteractInterrupted_Implementation()
{
}

void AMyInteractiveActor::Server_UseInterrupted_Implementation()
{
}

void AMyInteractiveActor::Client_UseInterrupted_Implementation()
{
}
