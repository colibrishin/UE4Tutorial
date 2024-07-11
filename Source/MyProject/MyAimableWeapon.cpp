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

	if (Result)
	{
		Client_UpdateAmmoDisplay();
	}
	
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
		MyCharacter->OnAttackEnded.AddUniqueDynamic(MyCharacter, &AMyCharacter::ResetAttack);

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
		GetHSpreadDegree(GetConsecutiveShots(), GetWeaponStatComponent()->GetHSpread()),
		-GetActorForwardVector()
	);
	
	Normal = Normal.RotateAngleAxis
	(
		GetVSpreadDegree(GetConsecutiveShots(), GetWeaponStatComponent()->GetVSpread()),
		GetActorUpVector()
	);

	LOG_FUNC_PRINTF(LogTemp, Warning, "HSpread: %f", GetHSpreadDegree(GetConsecutiveShots(), GetWeaponStatComponent()->GetHSpread()));

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

		// todo: unbind the fire when player drops.
		const auto& PlayerCharacter = Cast<AMyCharacter>(GetItemOwner());

		GetWorld()->GetTimerManager().SetTimer
				(
				 OnFireReadyTimerHandle,
				 this,
				 &AMyWeapon::OnFireRateTimed,
				 GetWeaponStatComponent()->GetFireRate(),
				 false
				);

		if (HasAuthority())
		{
			++ConsecutiveShots;
		}

		Client_Attack();
		Client_UpdateAmmoDisplay();

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

			Client_Reload();
		}

		return true;
	}

	return false;
}

void AMyAimableWeapon::OnFireRateTimed()
{
	Super::OnFireRateTimed();
}

void AMyAimableWeapon::OnReloadDone()
{
	Super::OnReloadDone();
	Client_UpdateAmmoDisplay();
}

float AMyAimableWeapon::GetHSpreadDegree(const float Point, const float OscillationRate, const float Min, const float Max)
{
	const float Radian = FMath::Sin(Point / (OscillationRate * PI));

	const float ClampQDH = FMath::Lerp
	(
		Min, 
		Max, 
		Radian
	);

	return FMath::RadiansToDegrees(ClampQDH);
}
float AMyAimableWeapon::GetVSpreadDegree(const float Point, const float OscillationRate, const float Min, const float Max)
{
	// todo: Better formula to control
	const float Radian = FMath::Cos(Point / FMath::Sqrt(Point)) *
		FMath::Sin(Point / OscillationRate * FMath::LogX(10, Point) * (Point / 10));
	
	const float ClampQDV = FMath::Lerp
	(
		Min,
		Max,
		Radian
	);

	return FMath::RadiansToDegrees(ClampQDV);
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

void AMyAimableWeapon::Client_UpdateAmmoDisplay_Implementation() const
{
	if (!IsValid(GetItemOwner()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Owner is not valid"));
		return;
	}

	if (GetItemOwner() != GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Owner is not a player, Skip"));
		return;
	}

	const auto& PlayerController = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld());

	if (!IsValid(PlayerController))
	{
		LOG_FUNC(LogTemp, Error, "PlayerController is not valid");
		return;
	}

	const auto& HUD = Cast<AMyInGameHUD>(PlayerController->GetHUD());

	if (!IsValid(HUD))
	{
		LOG_FUNC(LogTemp, Error, "HUD is not valid");
		return;
	}

	HUD->UpdateAmmo(
				GetWeaponStatComponent()->GetCurrentAmmoCount(),
				GetWeaponStatComponent()->GetRemainingAmmoCount());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Ammo updated"));
}

