// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterRequiredWidget.generated.h"

class AA_Character;
// This class does not need to be modified.
UINTERFACE()
class UCharacterRequiredWidget : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYPROJECT_API ICharacterRequiredWidget
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void DispatchCharacter(AA_Character* InCharacter) = 0;

};
