// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyAnimNotify_FootstepSound.h"

#include "GameFramework/PawnMovementComponent.h"

#include "MyProject/Actors/BaseClass/A_Character.h"

void UMyAnimNotify_FootstepSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const auto& Owner = MeshComp->GetOwner())
	{
		if (const auto& Character = Cast<AA_Character>(Owner))
		{
			if (Character->GetMovementComponent()->IsFalling())
			{
				return;
			}
		}
	}

	Super::Notify(MeshComp, Animation);
}
