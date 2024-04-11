// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/MyCollectable.h"
#include "MyItem.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyItem : public AMyCollectable
{
	GENERATED_BODY()

public:
	AMyItem();
	virtual bool Interact(class AMyCharacter* Character) override;

protected:
	virtual void PostInitializeComponents() override;

};
