// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ShapeAdjust.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UShapeAdjust : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYPROJECT_API IShapeAdjust
{
	GENERATED_BODY()
	
protected:
	float DefaultSizeMultiplier = 1.f;

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	void RefreshCollision( const FName& CollisionProfileName );
};
