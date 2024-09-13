// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ReloadObject.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UReloadObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYPROJECT_API IReloadObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual void Reload();
};
