// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyInteractiveActor.generated.h"

UCLASS()
class MYPROJECT_API AMyInteractiveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyInteractiveActor();

	virtual bool Interact(class AMyCharacter* Character) PURE_VIRTUAL(AMyInteractiveActor::Interact, return false;);
	virtual bool Use(class AMyCharacter* Character) PURE_VIRTUAL(AMyInteractiveActor::Use, return false;);

};
