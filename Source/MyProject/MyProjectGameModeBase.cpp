// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyProjectGameModeBase.h"

#include "MyCharacter.h"
#include "MyPawn.h"

AMyProjectGameModeBase::AMyProjectGameModeBase()
{
	// StaticClass, 컴파일 타임 타입
	// GetClass, 런타임 타입 (Base class 포인터)
	// DefaultPawnClass = AMyCharacter::StaticClass();

    static ConstructorHelpers::FClassFinder<AMyCharacter> BP_Char(TEXT("Blueprint'/Game/Blueprints/BPMyCharacter.BPMyCharacter_C'"));

	if (BP_Char.Succeeded())
	{
		DefaultPawnClass = BP_Char.Class;
	}
}
