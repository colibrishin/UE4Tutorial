// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyAnimNotify_FootstepSound.h"

#include "MyCharacter.h"

void UMyAnimNotify_FootstepSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const auto& Owner = MeshComp->GetOwner())
	{
		if (const auto& Character = Cast<AMyCharacter>(Owner))
		{
			if (Character->GetMovementComponent()->IsFalling())
			{
				return;
			}
		}
	}

	Super::Notify(MeshComp, Animation);
}
