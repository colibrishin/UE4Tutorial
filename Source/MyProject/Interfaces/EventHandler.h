// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/Private/Utilities.hpp"
#include "MyProject/Interfaces/EventableContext.h"
#include "MyProject/Private/Data.h"
#include "UObject/Interface.h"
#include "EventHandler.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEventHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYPROJECT_API IEventHandler
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void DoEvent( TScriptInterface<IEventableContext> InContext, const FParameters& InParameters ) const = 0;

};
