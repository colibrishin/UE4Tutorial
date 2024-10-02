// Fill out your copyright notice in the Description page of Project Settings.


#include "PickingUp.h"
#include "../Private/Utilities.hpp"

DEFINE_LOG_CATEGORY(LogPickableObject);

#include "MyProject/Components/C_PickUp.h"


// Add default functionality here for any IPickableObject functions that are not pure virtual.
void IPickingUp::PickUp(UC_PickUp* InPickUp) {}

void IPickingUp::Drop(UC_PickUp* InPickUp) {}