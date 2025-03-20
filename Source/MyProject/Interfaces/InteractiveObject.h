#pragma once

#include "CoreMinimal.h"

#include "InteractiveObject.generated.h"

class AA_Character;

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

	friend class UC_Interactive;

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UC_Interactive* GetInteractiveComponent() const;

	virtual void StartInteraction() = 0;

	virtual void StopInteraction() = 0;

	virtual bool StartClientInteraction( AA_Character* InInteractor ) const = 0;
	
	virtual bool StopClientInteraction() const = 0;

protected:

	FORCEINLINE bool DoesHavePredication() const { return bUsePredicate; }

	virtual bool PredicateInteraction( AA_Character* InInteractor ) = 0;

	virtual void Interaction() = 0;

	bool bUsePredicate = false;
	
	bool bDelay = false;
};
