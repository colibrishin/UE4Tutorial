// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyFragGrenade.h"

#include "MyCharacter.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "MyStatComponent.h"

AMyFragGrenade::AMyFragGrenade() : IsThrown(false), IsExploded(false)
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Grenade(TEXT("SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/G67_Grenade/SK_G67_X.SK_G67_X'"));

	SetSkeletalMesh();

	if (SK_Grenade.Succeeded())
	{
		GetSkeletalMeshComponent()->SetSkeletalMesh(SK_Grenade.Object);
	}
}

bool AMyFragGrenade::AttackImpl()
{
	const auto& Result = Super::AttackImpl();

	if (Result)
	{
		Throw();
	}

	return Result;
}

bool AMyFragGrenade::ReloadImpl()
{
	return false;
}

void AMyFragGrenade::Throw()
{
	ExecuteServer(this, 
				   &AMyFragGrenade::Multi_Throw,
				   &AMyFragGrenade::ThrowImpl);
}

void AMyFragGrenade::Multi_Throw_Implementation()
{
	ThrowImpl();
}

void AMyFragGrenade::ThrowImpl()
{
	if (IsExploded || IsThrown)
	{
		return;
	}

	// todo: object will be thrown at the bottom of the character
	Drop();

	GetSkeletalMeshComponent()->AddImpulse(FVector::ForwardVector * 1000.f, NAME_None, true);

	GetWorldTimerManager().SetTimer
	(
		OnExplosionTimerExpiredHandle,
		this,
		&AMyFragGrenade::OnExplosionTimerExpired,
		3.f,
		false
	);

	IsThrown = true;
}

void AMyFragGrenade::OnExplosionTimerExpired()
{
	IsExploded = true;

	// todo: explosion effect

	if (HasAuthority())
	{
		TArray<FOverlapResult> HitResults;

		GetWorld()->OverlapMultiByChannel
		(
			OUT HitResults,
			GetActorLocation(),
			FQuat::Identity,
			ECollisionChannel::ECC_Pawn,
			FCollisionShape::MakeSphere(500.f)
		);

		for (const auto& Result : HitResults)
		{
			if (const auto& Character = Cast<AMyCharacter>(Result.GetActor()))
			{
				const auto& Distance = FVector::Distance(Character->GetActorLocation(), GetActorLocation());
				const auto& Ratio = 1.f - Distance / 500.f;
				const auto& Damage = 100.f * Ratio;

				Character->GetPlayerState<AMyPlayerState>()->TakeDamage
				(
					Damage, 
					{}, 
					Cast<AMyPlayerController>(GetItemOwner()->GetOwner()), 
					GetItemOwner()
				);
			}
		}

		Destroy(true);
	}
}
