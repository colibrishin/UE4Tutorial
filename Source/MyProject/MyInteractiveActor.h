// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "GameFramework/Actor.h"
#include "MyInteractiveActor.generated.h"

class AMyCharacter;

UCLASS()
class MYPROJECT_API AMyInteractiveActor : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void Server_Interact(AMyCharacter* Character);

	UFUNCTION(Server, Reliable)
	void Server_Use(AMyCharacter* Character);

	UFUNCTION(Server, Reliable)
	void Server_InteractInterrupted();

	UFUNCTION(Server, Reliable)
	void Server_UseInterrupted();

	UFUNCTION(Client, Reliable)
	void Client_Interact(AMyCharacter* Character);

	UFUNCTION(Client, Reliable)
	void Client_Use(AMyCharacter* Character);

	UFUNCTION(Client, Reliable)
	void Client_InteractInterrupted();

	UFUNCTION(Client, Reliable)
	void Client_UseInterrupted();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Interact(AMyCharacter* Character);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Use(AMyCharacter* Character);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_InteractInterrupted();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_UseInterrupted();

protected:
	virtual void Server_Interact_Implementation(AMyCharacter* Character);
	virtual void Server_Use_Implementation(AMyCharacter* Character);
	virtual void Server_InteractInterrupted_Implementation();
	virtual void Server_UseInterrupted_Implementation();
	
	virtual void Client_Interact_Implementation(AMyCharacter* Character);
	virtual void Client_Use_Implementation(AMyCharacter* Character);
	virtual void Client_InteractInterrupted_Implementation();
	virtual void Client_UseInterrupted_Implementation();

	virtual void Multi_Interact_Implementation(AMyCharacter* Character);
	virtual void Multi_Use_Implementation(AMyCharacter* Character);
	virtual void Multi_InteractInterrupted_Implementation();
	virtual void Multi_UseInterrupted_Implementation();

};
