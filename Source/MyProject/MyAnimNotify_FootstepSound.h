// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "MyAnimNotify_FootstepSound.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyAnimNotify_FootstepSound : public UAnimNotify_PlaySound
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
