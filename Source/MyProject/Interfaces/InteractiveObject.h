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

	FORCEINLINE bool DoesHavePredication() const { return bUsePredicate; }

	virtual bool PredicateInteraction() = 0;

	virtual void Interaction() = 0;

	virtual void StopInteraction() = 0;

protected:
	bool bUsePredicate = false;
	
};
