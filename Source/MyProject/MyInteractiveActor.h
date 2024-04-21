// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "GameFramework/Actor.h"
#include "MyInteractiveActor.generated.h"

UCLASS()
class MYPROJECT_API AMyInteractiveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyInteractiveActor();

	virtual void Interact(class AMyCharacter* Character);

	virtual void Use(class AMyCharacter* Character);

	virtual void InteractInterrupted();

	virtual void UseInterrupted();


protected:
	virtual void UseImpl(class AMyCharacter* Character);

	virtual void InteractImpl(class AMyCharacter* Character);

	virtual void ClientInteractImpl(class AMyCharacter* Character);

	virtual void ClientUseImpl(class AMyCharacter* Character);

	virtual void InteractInterruptedImpl();

	virtual void ClientInteractInterruptedImpl();

	virtual void UseInterruptedImpl();

	virtual void ClientUseInterruptedImpl();

private:
	UFUNCTION(Server, Reliable)
	void Server_Interact(class AMyCharacter* Character);

	UFUNCTION(Client, Reliable)
	void Client_Interact(class AMyCharacter* Character);

	UFUNCTION(Server, Reliable)
	void Server_Use(class AMyCharacter* Character);

	UFUNCTION(Client, Reliable)
	void Client_Use(class AMyCharacter* Character);

	UFUNCTION(Server, Reliable)
	void Server_InteractInterrupted();

	UFUNCTION(Client, Reliable)
	void Client_InteractInterrupted();

	UFUNCTION(Server, Reliable)
	void Server_UseInterrupted();

	UFUNCTION(Client, Reliable)
	void Client_UseInterrupted();

};
