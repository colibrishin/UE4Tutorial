// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "MySpectatorPawn.generated.h"

class AA_Character;
/**
 * 
 */
UCLASS()
class MYPROJECT_API AMySpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()


public:
	AMySpectatorPawn();

	void SetPreviousCharacter(AA_Character* NewPreviousCharacter);
	TWeakObjectPtr<AA_Character> GetPreviousCharacter() const;

private:
	TWeakObjectPtr<AA_Character> PreviousCharacter;

};
