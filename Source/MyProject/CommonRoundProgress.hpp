#pragma once

#include "Utilities.hpp"
#include "GameFramework/CharacterMovementComponent.h"

#include "Enum.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"

inline void HandleRoundProgressChanged(AMyCharacter* MyCharacter, const EMyRoundProgress MyRoundProgress)
{
	if (MyRoundProgress == EMyRoundProgress::FreezeTime)
	{
		LOG_FUNC(LogTemp, Warning, "FreezeTime");

		if (IsValid(MyCharacter))
		{
			LOG_FUNC(LogTemp, Warning, "SetMovementMode to None");
			MyCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);
		}
		else
		{
			LOG_FUNC(LogTemp, Error, "MyCharacter is not valid");
		}
	}
	else if (MyRoundProgress == EMyRoundProgress::Playing)
	{
		LOG_FUNC(LogTemp, Warning, "Playing");

		if (IsValid(MyCharacter))
		{
			LOG_FUNC(LogTemp, Warning, "SetMovementMode to Walking");
			MyCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		else
		{
			LOG_FUNC(LogTemp, Error, "MyCharacter is not valid");
		}
	}
}
