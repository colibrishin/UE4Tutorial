// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Weapon.h"

#include "MyProject/Components/C_PickUp.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyProject/Actors/BaseClass/A_Character.h"
#include "../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Public/Net/UnrealNetwork.h"

#include "Kismet/GameplayStatics.h"

#include "MyProject/MyPlayerController.h"
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

	if (static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Weapon
		(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Blueprints/Inputs/InputContext/IMC_Weapon.IMC_Weapon'"));
		IMC_Weapon.Succeeded())
	{
		InputMapping = IMC_Weapon.Object;
	}

	if (static ConstructorHelpers::FObjectFinder<UInputAction> IA_Attack
		(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Inputs/IA_Attack.IA_Attack'"));
		IA_Attack.Succeeded())
	{
		AttackAction = IA_Attack.Object;
	}

	if (static ConstructorHelpers::FObjectFinder<UInputAction> IA_Reload
		(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Inputs/IA_Reload.IA_Reload'"));
		IA_Reload.Succeeded())
	{
		ReloadAction = IA_Reload.Object;
	}
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
		LOG_FUNC(LogWeaponComponent, Log, "Attach pick up delegate for weapon");
		PickUpComponent->OnObjectPickUp.AddUniqueDynamic(this , &UC_Weapon::HandlePickUp);
	}
}

void UC_Weapon::AttackImplementation()
{
	if (ValidateAttack())
	{
		bFiring = true;

		AmmoSpentInClip++;
		ConsecutiveShot++;

		LOG_FUNC_PRINTF(LogWeaponComponent, Log, "Weapon attack Server RPC, AmmoSpentInClip: %d, ConsecutiveShot: %d", AmmoSpentInClip, ConsecutiveShot);

		OnAttackStart.Broadcast(this);
		Multi_PlayAttackSound();
		Cast<IAttackObject>(GetOwner())->Attack();
	}
	else
	{
		Server_StopAttack();
	}
}

void UC_Weapon::Server_StopAttack_Implementation()
{
	StopAttackImplementation();
}

void UC_Weapon::StopAttackImplementation()
{
	if (bFiring)
	{
		bFiring = false;
		ConsecutiveShot = 0;
		OnAttackEnd.Broadcast(this);

		if (SprayTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(SprayTimerHandle);
		}
	}
}

void UC_Weapon::Server_Attack_Implementation()
{
	AttackImplementation();
}

void UC_Weapon::Server_Reload_Implementation()
{
	ReloadImplementation();
}

void UC_Weapon::ReloadImplementation()
{
	if (ValidateReload())
	{
		OnReloadStart.Broadcast(this);
		Multi_PlayReloadSound();
		Cast<IReloadObject>(GetOwner())->Reload();
	}
}

void UC_Weapon::Client_SetupPickupInput_Implementation(const AA_Character* InCharacter)
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(InCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping , 1);
		}

		if (UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(InCharacter->InputComponent))
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
}

void UC_Weapon::Client_SetupDropInput_Implementation(const AA_Character* InCharacter)
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(InCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
					UEnhancedInputLocalPlayerSubsystem>
				(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(InputMapping);
		}

		if (UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(InCharacter->InputComponent))
		{
			InputComponent->RemoveBinding(*AttackStartBinding);
			InputComponent->RemoveBinding(*AttackStopBinding);
			InputComponent->RemoveBinding(*ReloadBinding);
		}
	}
}

void UC_Weapon::ReloadClip()
{
	if (GetRemainingAmmo() > 0)
	{
		AmmoSpent       += AmmoSpentInClip;
		LoadedAmmo = FMath::Clamp(GetRemainingAmmo(), 0, AmmoPerClip);
		AmmoSpentInClip = 0;
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

void UC_Weapon::OnRep_OnAmmoUpdated()
{
	OnAmmoUpdated.Broadcast(GetRemainingAmmoInClip() , GetRemainingAmmoWithoutCurrentClip(), this);
}

void UC_Weapon::HandleAttackStart(UC_Weapon* /*InWeapon*/)
{
	bCanFire   = false;
	bCanReload = false;

	if (bCanSpray)
	{
		ensure(!SprayTimerHandle.IsValid());

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindRaw(&OnAttackEnd , &FOnAttackEnd::Broadcast , this);

		GetWorld()->GetTimerManager().SetTimer
				(
				 SprayTimerHandle ,
				 TimerDelegate ,
				 AttackRate ,
				 false);
	}
}

void UC_Weapon::HandleAttackEnd(UC_Weapon* /*InWeapon*/)
{
	bCanFire   = GetRemainingAmmoInClip() > 0;
	bCanReload = GetRemainingAmmo() > 0;

	if (SprayTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(SprayTimerHandle);
	}

	if (!bCanFire || !bFiring)
	{
		StopAttackImplementation();
	}
	else if (bCanFire)
	{
		if (bCanSpray)
		{
			AttackImplementation();
		}
	}
}

void UC_Weapon::HandleReloadStart(UC_Weapon* /*InWeapon*/)
{
	bCanFire   = false;
	bCanReload = false;

	ensure(!ReloadTimerHandle.IsValid());

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindRaw(&OnReloadEnd , &FOnReloadEnd::Broadcast , this);

	GetWorld()->GetTimerManager().SetTimer
			(
			 ReloadTimerHandle ,
			 TimerDelegate ,
			 ReloadTime ,
			 false
			);
}

void UC_Weapon::HandleReloadEnd(UC_Weapon* /*InWeapon*/)
{
	if (ReloadTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}

	ReloadClip();

	const bool AmmoLeft     = GetRemainingAmmoInClip() > 0;
	const bool MagazineLeft = GetRemainingAmmo() > 0;

	bCanFire   = AmmoLeft;
	bCanReload = MagazineLeft;
}

void UC_Weapon::HandlePickUp(TScriptInterface<IPickingUp> InPickUpObject)
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	bCanFire = GetRemainingAmmoInClip() > 0;
	bCanReload = GetRemainingAmmo() > 0;

	if (const AA_Character* Character = Cast<AA_Character>(InPickUpObject.GetInterface()))
	{
		Client_SetupPickupInput(Character);

		if (UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>())
		{
			PickUpComponent->OnObjectPickUp.RemoveAll(this);
			PickUpComponent->OnObjectDrop.AddUniqueDynamic(this , &UC_Weapon::HandleDrop);
		}
	}
}

void UC_Weapon::HandleDrop(TScriptInterface<IPickingUp> InPickUpObject)
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	if (const AA_Character* Character = Cast<AA_Character>(InPickUpObject.GetInterface()))
	{
		Client_SetupDropInput(Character);

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
