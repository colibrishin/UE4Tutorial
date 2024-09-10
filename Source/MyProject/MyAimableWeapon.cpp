// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyAimableWeapon.h"

#include "DrawDebugHelpers.h"
#include "MyCharacter.h"
#include "MyInGameHUD.h"
#include "NiagaraComponent.h"

#include "Algo/Rotate.h"

#include "Camera/CameraComponent.h"

#include "Components/BoxComponent.h"

#include "Kismet/GameplayStatics.h"

#include "NiagaraSystem.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

AMyAimableWeapon::AMyAimableWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	BulletTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletTrail"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_BulletTrail(TEXT("NiagaraSystem'/Game/Blueprints/BPNiagaraMyBulletTrail.BPNiagaraMyBulletTrail'"));

	if (NS_BulletTrail.Succeeded())
	{
		BulletTrail->SetAsset(NS_BulletTrail.Object);
	}

	SetSkeletalMesh();

	BulletTrail->SetupAttachment(GetMesh(), TEXT("Muzzle"));
	BulletTrail->SetAutoActivate(false);
	BulletTrail->SetAutoDestroy(false);
}

bool AMyAimableWeapon::PreInteract(AMyCharacter* Character)
{
	return Super::PreInteract(Character);
}

bool AMyAimableWeapon::PostInteract(AMyCharacter* Character)
{
	const auto& Result = Super::PostInteract(Character);
	return Result;
}

bool AMyAimableWeapon::TryAttachItem(AMyCharacter* Character)
{
	LOG_FUNC(LogTemp, Warning, "TryAttachItem");

	if (GetMesh()->AttachToComponent
		(
		 Character->GetMesh(),
		 FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		 AMyCharacter::RightHandSocketName
		))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("AttachToComponent success"));

		const auto& MyCharacter = GetItemOwner();

		MyCharacter->OnInteractInterrupted.AddUniqueDynamic(this, &AMyCollectable::Server_InteractInterrupted);
		MyCharacter->OnUseInterrupted.AddUniqueDynamic(this, &AMyCollectable::Server_UseInterrupted);
		OnFireReady.AddUniqueDynamic(MyCharacter, &AMyCharacter::ResetAttack);

		Client_TryAttachItem(Character);

		return true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("AttachToComponent failed"));
		return false;
	}
}

bool AMyAimableWeapon::PreUse(AMyCharacter* Character)
{
	return Super::PreUse(Character);
}

bool AMyAimableWeapon::PostUse(AMyCharacter* Character)
{
	return Super::PostUse(Character);
}

bool AMyAimableWeapon::Hitscan(const FVector& Position, const FVector& Forward, FHitResult& OutHitResult)
{
	if (!GetWeaponStatComponent()->IsHitscan())
	{
		LOG_FUNC(LogTemp, Warning, "Not a hitscan weapon");
		return false;
	}

	if (IsDummyVisually())
	{
		return false;
	}

	LOG_FUNC(LogTemp, Warning, "Hitscan Fire!");

	FCollisionQueryParams Params{NAME_None, false, this};
	Params.AddIgnoredActor(GetItemOwner());
	Params.AddIgnoredActor(GetItemOwner()->GetCurrentHand());

	Normal = Forward.RotateAngleAxis
	(
		GetCurveValue(GetWeaponStatComponent()->GetHSpread(), GetConsecutiveShots(), GetWeaponStatComponent()->GetAmmoPerLoad()),
		-GetActorUpVector()
	);
	
	Normal = Normal.RotateAngleAxis
	(
		GetCurveValue(GetWeaponStatComponent()->GetVSpread(), GetConsecutiveShots(), GetWeaponStatComponent()->GetAmmoPerLoad()),
		GetActorForwardVector()
	);

	LOG_FUNC_PRINTF(LogTemp, Warning, "Normal: %s", *Normal.ToString());

	const FVector EndVector = 
		Position + (Normal * GetWeaponStatComponent()->GetRange());

	const auto& Result = GetWorld()->LineTraceSingleByChannel
		(
		 OUT OutHitResult,
		 Position,
		 EndVector,
		 ECollisionChannel::ECC_Visibility,
		 Params
		);

	if (Result)
	{
		DrawDebugBox
		(
			GetWorld(),
			OutHitResult.ImpactPoint,
			FVector(1.f),
			FColor::Red,
			false,
			5.f
		);
	}

	return Result;
}

void AMyAimableWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMyAimableWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyAimableWeapon, Normal);
}

void AMyAimableWeapon::Client_TryAttachItem_Implementation(AMyCharacter* Character)
{
	Super::Client_TryAttachItem_Implementation(Character);
	OnFireReady.AddUniqueDynamic(Character, &AMyCharacter::ResetAttack);
}

void AMyAimableWeapon::DropBeforeCharacter()
{
	Super::DropBeforeCharacter();
	Client_DropBeforeCharacter();
}

void AMyAimableWeapon::Client_DropBeforeCharacter_Implementation()
{
	const auto& Character = Cast<AMyCharacter>(GetItemOwner());
	OnFireReady.RemoveDynamic(Character, &AMyCharacter::ResetAttack);
}

void AMyAimableWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (const auto& StatComponent = GetWeaponStatComponent())
	{
		BulletTrail->SetNiagaraVariableFloat(TEXT("User.FireRate"), StatComponent->GetFireRate());
	}
}

bool AMyAimableWeapon::AttackImpl()
{
	if (GetWeaponStatComponent()->ConsumeAmmo())
	{
		// todo: Visual Recoil

		const auto& PawnCamera = GetItemOwner()->FindComponentByClass<UCameraComponent>();
		const auto& CameraLocation = PawnCamera->GetComponentLocation();
		const auto& CameraForward = PawnCamera->GetForwardVector();

		FHitResult HitResult;

		// Check hit in server side.
		if (!IsDummyVisually() && HasAuthority() && Hitscan(CameraLocation, CameraForward, HitResult))
		{
			if (const auto& Character = Cast<AMyCharacter>(HitResult.GetActor())) 
			{
				Character->TakeDamage
				(
					GetWeaponStatComponent()->GetDamage(), 
					FDamageEvent{}, 
					GetItemOwner()->GetController(), 
					this
				);
			}
		}

		Multi_TriggerBulletTrail();

		if (IsDummyVisually())
		{
			return false;
		}

		const auto& PlayerCharacter = Cast<AMyCharacter>(GetItemOwner());

		if (!OnFireReadyTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer
				(
				 OnFireReadyTimerHandle,
				 this,
				 &AMyWeapon::OnFireRateTimed,
				 GetWeaponStatComponent()->GetFireRate(),
				 false
				);
		}

		// Note that ConsecutiveShots is replicated.
		++ConsecutiveShots;

		Client_PlayAttackSound();
		Client_Attack();

		return true;
	}

	return false;
}

bool AMyAimableWeapon::ReloadImpl()
{
	if (GetWeaponStatComponent()->GetRemainingAmmoCount() > 0 && 
		 GetWeaponStatComponent()->GetCurrentAmmoCount() != GetWeaponStatComponent()->GetLoadedAmmoCount())
	{
		GetWeaponStatComponent()->Reload();

		if (IsDummyVisually())
		{
			return false;
		}

		if (!OnReloadDoneTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer
				(
				 OnReloadDoneTimerHandle ,
				 this ,
				 &AMyWeapon::OnReloadDone ,
				 GetWeaponStatComponent()->GetReloadTime() ,
				 false
				);

			Client_PlayReloadSound();
			Client_Reload();
		}

		return true;
	}

	return false;
}

void AMyAimableWeapon::OnFireRateTimed()
{
	Super::OnFireRateTimed();
	OnFireReadyTimerHandle.Invalidate();
}

void AMyAimableWeapon::OnReloadDone()
{
	Super::OnReloadDone();
	OnReloadDoneTimerHandle.Invalidate();
}

float AMyAimableWeapon::GetCurveValue(
	const UCurveFloat* InCurve, const float InValue, const float InValueMax, const float Min, const float Max
)
{
	return FMath::RadiansToDegrees(FMath::Sin(FMath::Wrap(InCurve->GetFloatValue(InValue / InValueMax), Min, Max)));
}

void AMyAimableWeapon::Client_Reload_Implementation()
{
	GetWorld()->GetTimerManager().SetTimer
			(
			 OnReloadDoneTimerHandle ,
			 this ,
			 &AMyWeapon::OnReloadDone ,
			 GetWeaponStatComponent()->GetReloadTime() ,
			 false
			);
}

void AMyAimableWeapon::Client_Attack_Implementation()
{
	LOG_FUNC(LogTemp, Log, "Client side attack triggered");

	GetWorld()->GetTimerManager().SetTimer
			(
			 OnFireReadyTimerHandle ,
			 this ,
			 &AMyWeapon::OnFireRateTimed ,
			 GetWeaponStatComponent()->GetFireRate() ,
			 false
			);
}

void AMyAimableWeapon::Multi_TriggerBulletTrail_Implementation()
{
	const auto& PawnCamera = GetItemOwner()->FindComponentByClass<UCameraComponent>();
	const auto& CameraLocation = PawnCamera->GetComponentLocation();
	const auto& MuzzleLocation = GetMesh()->GetSocketLocation(TEXT("Muzzle"));
	const auto& WeaponRange = GetWeaponStatComponent()->GetRange();

	FVector TrailNormal = Normal;

	if (IsDummyVisually())
	{
		const auto& MainWeapon = GetItemOwner()->TryGetWeapon();
		TrailNormal = Cast<AMyAimableWeapon>(MainWeapon)->Normal;
	}

	const auto& EndLocation = CameraLocation + (TrailNormal * WeaponRange);
	const auto& Delta       = EndLocation - MuzzleLocation;
	const auto& DeltaNormal = Delta.GetSafeNormal();

	const auto& Yaw = FMath::RadiansToDegrees(FMath::Atan2(DeltaNormal.Y, DeltaNormal.X));
	const auto& Pitch = FMath::RadiansToDegrees(FMath::Atan2(DeltaNormal.Z, DeltaNormal.X));
	const auto& Roll = 0.f;

	BulletTrail->SetNiagaraVariableFloat(TEXT("User.Yaw"), Yaw);
	BulletTrail->SetNiagaraVariableFloat(TEXT("User.Pitch"), Pitch);
	BulletTrail->SetNiagaraVariableFloat(TEXT("User.Roll"), Roll);
	BulletTrail->Activate();
}

