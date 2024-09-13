#pragma once

#include "CoreMinimal.h"

#include "InteractiveObject.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInteractiveObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYPROJECT_API IInteractiveObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Interaction();
};
