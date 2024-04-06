// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyProjectGameModeBase.h"

#include "MyCharacter.h"
#include "MyPawn.h"

AMyProjectGameModeBase::AMyProjectGameModeBase()
{
	// StaticClass, ������ Ÿ�� Ÿ��
	// GetClass, ��Ÿ�� Ÿ�� (Base class ������)
	// DefaultPawnClass = AMyCharacter::StaticClass();

    static ConstructorHelpers::FClassFinder<AMyCharacter> BP_Char(TEXT("Blueprint'/Game/Blueprints/BPMyCharacter.BPMyCharacter_C'"));

	if (BP_Char.Succeeded())
	{
		DefaultPawnClass = BP_Char.Class;
	}
}
