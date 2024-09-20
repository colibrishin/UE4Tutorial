// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Weapon.h"

#include "MyProject/Components/C_PickUp.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyProject/Actors/BaseClass/A_Character.h"
#include "../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Public/Net/UnrealNetwork.h"

#include "Kismet/GameplayStatics.h"

#include "MyProject/Private/Utilities.hpp"

#include "MyProject/Interfaces/AttackObject.h"
#include "MyProject/Interfaces/ReloadObject.h"

DEFINE_LOG_CATEGORY(LogWeaponComponent);

// Sets default values for this component's properties
UC_Weapon::UC_Weapon()
	: bFiring(false),
	  bCanReload(false),
	  bCanFire(false),
	  bCanSpray(false),
	  AttackRate(1.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	OnAttackStart.AddUniqueDynamic(this , &UC_Weapon::HandleAttackStart);
	OnAttackEnd.AddUniqueDynamic(this , &UC_Weapon::HandleAttackEnd);
	OnReloadStart.AddUniqueDynamic(this , &UC_Weapon::HandleReloadStart);
	OnReloadEnd.AddUniqueDynamic(this , &UC_Weapon::HandleReloadEnd);
}


uint32 UC_Weapon::GetRemainingAmmo() const
{
	return TotalAmmo - AmmoSpent;
}

uint32 UC_Weapon::GetRemainingAmmoInClip() const
{
	return LoadedAmmo - AmmoSpentInClip;
}

uint32 UC_Weapon::GetRemainingAmmoWithoutCurrentClip() const
{
	return GetRemainingAmmo() - LoadedAmmo;
}

uint32 UC_Weapon::GetAmmoPerClip() const
{
	return AmmoPerClip;
}

uint32 UC_Weapon::GetConsecutiveShot() const
{
	return ConsecutiveShot;
}

void UC_Weapon::Attack()
{
	if (ValidateAttack())
	{
		OnAttackStart.Broadcast(this);
		Server_Attack();
	}
}

void UC_Weapon::StopAttack()
{
	if (bFiring)
	{
		Server_StopAttack();
	}
}

void UC_Weapon::Reload()
{
	if (ValidateReload())
	{
		OnReloadStart.Broadcast(this);
		Server_Reload();
	}
}

// Called when the game starts
void UC_Weapon::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>())
	{
		PickUpComponent->OnObjectPickUp.AddUniqueDynamic(this , &UC_Weapon::HandlePickUp);
	}
	else
	{
		LOG_FUNC
		(LogWeaponComponent , Error , "Unable to find the pick up component, weapon will not be able to pick up!");
	}
}

void UC_Weapon::Server_StopAttack_Implementation()
{
	if (bFiring)
	{
		bFiring = false;
		ConsecutiveShot = 0;
		
		if (SprayTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(SprayTimerHandle);
		}
	}
}

void UC_Weapon::Server_Attack_Implementation()
{
	if (ValidateAttack())
	{
		if (!bFiring)
		{
			bFiring = true;

			if (bCanSpray)
			{
				GetWorld()->GetTimerManager().SetTimer
					(
					 SprayTimerHandle ,
					 this ,
					 &UC_Weapon::Server_Attack ,
					 AttackRate ,
					 true
					);
			}
		}

		AmmoSpentInClip++;
		ConsecutiveShot++;
		OnAttackStart.Broadcast(this);
		Multi_PlayAttackSound();
		Cast<IAttackObject>(GetOwner())->Attack();
	}
	else
	{
		Server_StopAttack();
	}
}

void UC_Weapon::Server_Reload_Implementation()
{
	if (ValidateReload())
	{
		OnReloadStart.Broadcast(this);
		Multi_PlayReloadSound();
		Cast<IReloadObject>(GetOwner())->Reload();
	}
}

bool UC_Weapon::ValidateAttack()
{
	// Cannot fire (recovery time); Is reloading or no ammo left;
	if (!bCanFire || !bCanReload)
	{
		LOG_FUNC_PRINTF
		(LogWeaponComponent , Log , "Cannot fire; bCanFire: %d; bCanReload: %d;" , bCanFire , bCanReload);
		return false;
	}

	IAttackObject* AttackObject = Cast<IAttackObject>(GetOwner());

	if (!AttackObject)
	{
		LOG_FUNC_PRINTF
		(LogWeaponComponent , Error , "Trying to attack with no attack object %s;" , *GetOwner()->GetName());
		return false;
	}

	return true;
}

bool UC_Weapon::ValidateReload()
{
	// Is firing or no ammo left;
	if (!bCanReload)
	{
		LOG_FUNC_PRINTF
		(LogWeaponComponent , Log , "Cannot reload; Ammo spent: %d; Total ammo: %d" , AmmoSpent , TotalAmmo);
		return false;
	}

	IReloadObject* ReloadObject = Cast<IReloadObject>(GetOwner());

	if (!ReloadObject)
	{
		LOG_FUNC_PRINTF
		(LogWeaponComponent , Error , "Attempt to reload with not reload object %s;" , *GetOwner()->GetName())
		return false;
	}

	if (!GetRemainingAmmo())
	{
		LOG_FUNC_PRINTF
		(LogWeaponComponent , Log , "No ammo left; Ammo spent: %d; Total ammo: %d" , AmmoSpent , TotalAmmo);
		return false;
	}

	return true;
}

void UC_Weapon::OnRep_OnAmmoUpdated(const int32 InOld)
{
	OnAmmoUpdated.Broadcast(InOld , AmmoSpent , this);
}

void UC_Weapon::HandleAttackStart(UC_Weapon* /*InWeapon*/)
{
	bCanFire   = false;
	bCanReload = false;
}

void UC_Weapon::HandleAttackEnd(UC_Weapon* /*InWeapon*/)
{
	const bool AmmoLeft     = GetRemainingAmmo() > 0;
	const bool MagazineLeft = LoadedAmmo > AmmoSpentInClip;

	if (!(AmmoLeft && MagazineLeft))
	{
		Server_StopAttack();
	}

	bCanFire   = AmmoLeft && MagazineLeft;
	bCanReload = AmmoLeft && MagazineLeft;
}

void UC_Weapon::HandleReloadStart(UC_Weapon* /*InWeapon*/)
{
	bCanFire   = false;
	bCanReload = false;
}

void UC_Weapon::HandleReloadEnd(UC_Weapon* /*InWeapon*/)
{
	bCanFire   = true;
	bCanReload = true;

	AmmoSpent       += AmmoSpentInClip;
	AmmoSpentInClip = 0;
	LoadedAmmo = FMath::Clamp(GetRemainingAmmo(), 0, AmmoPerClip);
}

void UC_Weapon::HandlePickUp(TScriptInterface<IPickingUp> InPickUpObject)
{
	if (const ACharacter* Character = Cast<ACharacter>(InPickUpObject.GetInterface()))
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
					UEnhancedInputLocalPlayerSubsystem>
				(PlayerController->GetLocalPlayer())) { Subsystem->AddMappingContext(InputMapping , 1); }

			if (UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(Character->InputComponent))
			{
				AttackStartBinding = &InputComponent->BindAction
					(AttackAction , ETriggerEvent::Started , this , &UC_Weapon::Attack);
				AttackStopBinding = &InputComponent->BindAction
					(AttackAction , ETriggerEvent::Completed , this , &UC_Weapon::StopAttack);

				ReloadBinding = &InputComponent->BindAction
					(ReloadAction , ETriggerEvent::Started , this , &UC_Weapon::Reload);
			}
			else
			{
				LOG_FUNC(LogWeaponComponent , Error , "Unable to bind the key binding");
			}
		}
		if (UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>())
		{
			PickUpComponent->OnObjectPickUp.RemoveAll(this);
			PickUpComponent->OnObjectDrop.AddUniqueDynamic(this , &UC_Weapon::HandleDrop);
		}
	}
}

void UC_Weapon::HandleDrop(TScriptInterface<IPickingUp> InPickUpObject)
{
	if (const ACharacter* Character = Cast<ACharacter>(InPickUpObject.GetInterface()))
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
					UEnhancedInputLocalPlayerSubsystem>
				(PlayerController->GetLocalPlayer())) { Subsystem->RemoveMappingContext(InputMapping); }

			if (UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(Character->InputComponent))
			{
				InputComponent->RemoveBinding(*AttackStartBinding);
				InputComponent->RemoveBinding(*AttackStopBinding);
				InputComponent->RemoveBinding(*ReloadBinding);
			}
		}
		
		if (UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>())
		{
			PickUpComponent->OnObjectPickUp.AddUniqueDynamic(this , &UC_Weapon::HandlePickUp);
			PickUpComponent->OnObjectDrop.RemoveAll(this);
		}
	}
}

void UC_Weapon::Multi_PlayAttackSound_Implementation()
{
	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld() , AttackSound , GetOwner()->GetActorLocation());
	}
}

void UC_Weapon::Multi_PlayReloadSound_Implementation()
{
	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld() , ReloadSound , GetOwner()->GetActorLocation());
	}
}

void UC_Weapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UC_Weapon , bFiring);
	DOREPLIFETIME(UC_Weapon , bCanReload);
	DOREPLIFETIME(UC_Weapon , bCanFire);
	DOREPLIFETIME(UC_Weapon , AmmoSpent);
	DOREPLIFETIME(UC_Weapon , AmmoSpentInClip);
	DOREPLIFETIME(UC_Weapon , LoadedAmmo);
}


// Called every frame
void UC_Weapon::TickComponent(float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	// ...
}
