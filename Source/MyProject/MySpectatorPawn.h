// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "MySpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMySpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()


public:
	AMySpectatorPawn();

	void SetPreviousCharacter(class AMyCharacter* NewPreviousCharacter);
	TWeakObjectPtr<class AMyCharacter> GetPreviousCharacter() const;

private:
	TWeakObjectPtr<class AMyCharacter> PreviousCharacter;

};
