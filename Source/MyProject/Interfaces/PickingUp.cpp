// Fill out your copyright notice in the Description page of Project Settings.


#include "PickingUp.h"
#include "../Private/Utilities.hpp"

#include "MyProject/Components/C_PickUp.h"

DEFINE_LOG_CATEGORY(LogPickableObject);

// Add default functionality here for any IPickableObject functions that are not pure virtual.
void IPickingUp::PickUp(UC_PickUp* InPickUp , const PickUpSpawnedPredicate& InSpawnPredicate , const PickUpSpawnedPredicate& InPostSpawnPredicate ) { }

void IPickingUp::Drop(UC_PickUp* InPickUp) {}