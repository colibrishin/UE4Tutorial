// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MySpectatorPawn.h"
#include "MyCharacter.h"

AMySpectatorPawn::AMySpectatorPawn()
{
}

void AMySpectatorPawn::SetPreviousCharacter(AMyCharacter* NewPreviousCharacter)
{
	PreviousCharacter = NewPreviousCharacter;
}

TWeakObjectPtr<AMyCharacter> AMySpectatorPawn::GetPreviousCharacter() const
{
	return PreviousCharacter;
}
