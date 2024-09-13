// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyFragGrenade.h"

#include "MyCharacter.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "MyProject/Components/MyStatComponent.h"
#include "MyProject/Components/MyWeaponStatComponent.h"

#include "Engine/OverlapResult.h"
#include "Engine/DamageEvents.h"

AMyFragGrenade::AMyFragGrenade()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Grenade(TEXT("SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/G67_Grenade/SK_G67_X.SK_G67_X'"));

	if (SK_Grenade.Succeeded())
	{
		GetSkeletalMeshComponent()->SetSkeletalMesh(SK_Grenade.Object);
	}
}

void AMyFragGrenade::OnExplosionTimerExpiredImpl()
{
	Super::OnExplosionTimerExpiredImpl();

	if (HasAuthority())
	{
		TArray<FOverlapResult> HitResults;

		const auto& Radius = GetWeaponStatComponent()->GetRadius();
		const auto& Damage = GetWeaponStatComponent()->GetDamage();

		GetWorld()->OverlapMultiByChannel
		(
			OUT HitResults,
			GetActorLocation(),
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeSphere(Radius)
		);

		if (!IsValid(GetPreviousOwner()))
		{
			LOG_FUNC(LogTemp, Error, "Previous owner is not noted");
			return;
		}

		for (const auto& Result : HitResults)
		{
			if (const auto& Character = Cast<AMyCharacter>(Result.GetActor()))
			{
				const auto& Distance = FVector::Distance(Character->GetActorLocation(), GetActorLocation());
				const auto& Ratio = 1.f - Distance / Radius;
				const auto& RatioDamage = Damage * Ratio;

				if (const auto& MyPlayerState = Character->GetPlayerState<AMyPlayerState>())
				{
					MyPlayerState->TakeDamage
					(
						RatioDamage, 
						FDamageEvent{}, 
						Cast<AMyPlayerController>(GetPreviousOwner()->GetOwner()), 
						GetPreviousOwner()
					);
				}
			}
		}
	}

	Destroy(true);
}
