// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PickingUp.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPickableObject, All, Log);

class UC_PickUp;
// This class does not need to be modified.
UINTERFACE()
class UPickingUp : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYPROJECT_API IPickingUp
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void PickUp(UC_PickUp* InPickUp);

	virtual void Drop(UC_PickUp* InPickUp);

private:
	void ClientCheck(const UC_PickUp* InPickUp) const;
	
};
