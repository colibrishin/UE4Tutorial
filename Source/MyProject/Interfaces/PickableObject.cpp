// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableObject.h"
#include "../Private/Utilities.hpp"

DEFINE_LOG_CATEGORY(LogPickableObject);

#include "MyProject/Components/C_PickUp.h"


// Add default functionality here for any IPickableObject functions that are not pure virtual.
void IPickableObject::PickUp(UC_PickUp* InPickUp)
{
	ClientCheck(InPickUp);
}

void IPickableObject::Drop(UC_PickUp* InPickUp)
{
	ClientCheck(InPickUp);
}

void IPickableObject::ClientCheck(const UC_PickUp* InPickUp) const
{
	if (InPickUp->GetWorld()->GetNetMode() == NM_Client)
	{
		LOG_FUNC_PRINTF(LogPickableObject, Warning, "%s is called in client context", *InPickUp->GetName());
	}
}
