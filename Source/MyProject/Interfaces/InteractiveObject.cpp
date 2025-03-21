#include "InteractiveObject.h"
#include <MyProject/Components/C_Interactive.h>


// Add default functionality here for any IInteractiveObject functions that are not pure virtual.

UC_Interactive* IInteractiveObject::GetInteractiveComponent() const
{
	UC_Interactive* Component = nullptr;
	if (const AActor* Actor = Cast<AActor>( this ) )
	{
		Component = Actor->GetComponentByClass<UC_Interactive>();
	}

	return Component;
}
