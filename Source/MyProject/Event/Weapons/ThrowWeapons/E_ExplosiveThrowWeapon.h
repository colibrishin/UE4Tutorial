// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyProject/Interfaces/EventHandler.h"
#include "E_ExplosiveThrowWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UE_ExplosiveThrowWeapon : public UObject, public IEventHandler
{
	GENERATED_BODY()

public:
	virtual void DoEvent( TScriptInterface<IEventableContext> InContext, const FParameters& InParameters ) const override;
	
};
