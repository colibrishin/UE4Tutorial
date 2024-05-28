// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/MyGrenade.h"
#include "MySmokeGrenade.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMySmokeGrenade : public AMyGrenade
{
	GENERATED_BODY()

public:
	AMySmokeGrenade();
	
protected:
	void OnExplosionTimerExpiredImpl() override;

	void OnSmokeEffectExpired();

private:
	FTimerHandle OnSmokeEffectExpiredHandle;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	class UNiagaraComponent* SmokeEffect;
	
};
