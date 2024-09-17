// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MySpectatorPawn.h"

#include "Actors/BaseClass/A_Character.h"

AMySpectatorPawn::AMySpectatorPawn()
{
}

void AMySpectatorPawn::SetPreviousCharacter(AA_Character* NewPreviousCharacter)
{
	PreviousCharacter = NewPreviousCharacter;
}

TWeakObjectPtr<AA_Character> AMySpectatorPawn::GetPreviousCharacter() const
{
	return PreviousCharacter;
}
