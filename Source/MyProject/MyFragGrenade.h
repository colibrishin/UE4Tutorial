// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGrenade.h"

#include "MyFragGrenade.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyFragGrenade : public AMyGrenade
{
	GENERATED_BODY()

public:
    AMyFragGrenade();

protected:
	virtual void OnExplosionTimerExpiredImpl() override;

};
