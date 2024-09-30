// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Weapon.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "MyProject/Components/C_PickUp.h"

#include "../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Public/Net/UnrealNetwork.h"
#include "MyProject/Actors/BaseClass/A_Character.h"

#include "Kismet/GameplayStatics.h"

#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Actors/BaseClass/A_Weapon.h"
#include "MyProject/Private/Utilities.hpp"

#include "MyProject/Interfaces/AttackObject.h"
#include "MyProject/Interfaces/ReloadObject.h"

DEFINE_LOG_CATEGORY(LogWeaponComponent);

// Sets default values for this component's properties
UC_Weapon::UC_Weapon()
	: bFiring(false),
	  bReloading(false),
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
	OnStopAttack.AddUniqueDynamic(this, &UC_Weapon::HandleStopAttack);
	
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
		Server_Reload();
	}
}

bool UC_Weapon::IsDummy() const
{
	const AA_Collectable* Collectable = Cast<AA_Collectable>(GetOwner());
	ensure(Collectable);
	return Collectable->IsDummy();
}

UC_Weapon* UC_Weapon::GetSiblingComponent() const
{
	const AA_Weapon* Collectable = Cast<AA_Weapon>(GetOwner());
	ensure(Collectable);
	const AA_Weapon* Sibling = Cast<AA_Weapon>(Collectable->GetSibling());

	if (!Sibling)
	{
		return nullptr;
	}
	
	return Sibling->GetWeaponComponent();
}

// Called when the game starts
void UC_Weapon::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>())
	{
		LOG_FUNC_PRINTF(LogWeaponComponent, Log, "Attach pick up delegate for weapon %s", *GetOwner()->GetName());
		PickUpComponent->OnObjectPickUp.AddUniqueDynamic(this , &UC_Weapon::HandlePickUp);
	}

	if (AA_Collectable* Collectable = Cast<AA_Collectable>(GetOwner()))
	{
		Collectable->OnDummyFlagSet.AddUniqueDynamic(this, &UC_Weapon::HandleDummy);
	}

	if (IAttackObject* AttackObject = Cast<IAttackObject>(GetOwner()))
	{
		OnAttack.AddUniqueDynamic(AttackObject, &IAttackObject::Attack);
	}

	if (IReloadObject* ReloadObject = Cast<IReloadObject>(GetOwner()))
	{
		OnReload.AddUniqueDynamic(ReloadObject, &IReloadObject::Reload);
	}
}

#pragma region Network
void UC_Weapon::Server_StopAttack_Implementation()
{
	StopAttackImplementation();
}

void UC_Weapon::Server_Attack_Implementation()
{
	AttackImplementation();
}

void UC_Weapon::Server_Reload_Implementation()
{
	ReloadImplementation();
}

void UC_Weapon::Client_SetupPickupInput_Implementation(const AA_Character* InCharacter)
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(InCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping , IsDummy() ? 1 : 0);
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

void UC_Weapon::OnRep_OnAmmoUpdated()
{
	OnAmmoUpdated.Broadcast(GetRemainingAmmoInClip() , GetRemainingAmmoWithoutCurrentClip(), this);
}
#pragma endregion 

void UC_Weapon::AttackImplementation()
{
	if (ValidateAttack())
	{
		bFiring = true;
		// Update state, consume ammo, Play sound, animation etc...
		OnAttackStart.Broadcast(this);
		// Process for attack
		OnAttack.Broadcast();
	}
	else
	{
		StopAttackImplementation();
	}
}

void UC_Weapon::StopAttackImplementation()
{
	if (bFiring)
	{
		bFiring = false;
		OnStopAttack.Broadcast(this);
	}
}

void UC_Weapon::ReloadImplementation()
{
	if (ValidateReload())
	{
		bReloading = true;

		// Update state, play sound, animation etc...
		OnReloadStart.Broadcast(this);
		// Process for reload.
		OnReload.Broadcast();
	}
}

bool UC_Weapon::ValidateAttack()
{
	// Cannot fire (recovery time); Is reloading or no ammo left;
	if (!bCanFire || !bCanReload)
	{
		LOG_FUNC_PRINTF
		(LogWeaponComponent , Log , "Cannot fire; bCanFire: %d; bCanReload: %d; Dummy: %d;" , bCanFire , bCanReload, IsDummy());
		return false;
	}

	ensure(Cast<IAttackObject>(GetOwner()));
	return true;
}

bool UC_Weapon::ValidateReload()
{
	// Is firing or no ammo left;
	if (!bCanReload)
	{
		LOG_FUNC_PRINTF
		(LogWeaponComponent , Log , "Cannot reload; Ammo spent: %d; Total ammo: %d Dummy: %d;" , AmmoSpent , TotalAmmo, IsDummy());
		return false;
	}

	ensure(Cast<IReloadObject>(GetOwner()));

	if (!GetRemainingAmmo())
	{
		LOG_FUNC_PRINTF
		(LogWeaponComponent , Log , "No ammo left; Ammo spent: %d; Total ammo: %d Dummy: %d;" , AmmoSpent , TotalAmmo, IsDummy());
		return false;
	}

	return true;
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

	if (!bCanFire)
	{
		StopAttackImplementation();
		return;
	}
	
	if (bFiring)
	{
		if (bCanSpray)
		{
			if (SprayTimerHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(SprayTimerHandle);
			}
			
			AttackImplementation();
		}
		else
		{
			StopAttackImplementation();
		}
	}
}

void UC_Weapon::HandleStopAttack(UC_Weapon* /*InWeapon*/)
{
	bCanFire   = GetRemainingAmmoInClip() > 0;
	bCanReload = GetRemainingAmmo() > 0;

	ConsecutiveShot = 0;
	
	if (SprayTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(SprayTimerHandle);
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
	bReloading = false;
	
	const bool AmmoLeft     = GetRemainingAmmoInClip() > 0;
	const bool MagazineLeft = GetRemainingAmmo() > 0;

	bCanFire   = AmmoLeft;
	bCanReload = MagazineLeft;
}

void UC_Weapon::HandlePickUp(TScriptInterface<IPickingUp> InPickUpObject, const bool bCallPickUp)
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	bCanFire = GetRemainingAmmoInClip() > 0;
	bCanReload = GetRemainingAmmo() > 0;

	if (const AA_Character* Character = Cast<AA_Character>(InPickUpObject.GetInterface()))
	{
		if (!IsDummy() && Character->Controller == GetWorld()->GetFirstPlayerController())
		{
			Client_SetupPickupInput(Character);	
		}

		if (UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>())
		{
			PickUpComponent->OnObjectPickUp.RemoveAll(this);
			PickUpComponent->OnObjectDrop.AddUniqueDynamic(this , &UC_Weapon::HandleDrop);
		}
	}
}

void UC_Weapon::HandleDrop(TScriptInterface<IPickingUp> InPickUpObject, const bool bCallDrop)
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	if (const AA_Character* Character = Cast<AA_Character>(InPickUpObject.GetInterface()))
	{
		// todo/fixme : possible unhandled condition
		// spawn normally -> pick up -> set dummy during picked up -> input binding still persist;

		if (!IsDummy() && Character->GetController() == GetWorld()->GetFirstPlayerController())
		{
			Client_SetupDropInput(Character);	
		}

		if (UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>())
		{
			PickUpComponent->OnObjectPickUp.AddUniqueDynamic(this , &UC_Weapon::HandlePickUp);
			PickUpComponent->OnObjectDrop.RemoveAll(this);
		}
	}
}

void UC_Weapon::ConsumeAmmo()
{
	bFiring = true;
	AmmoSpentInClip++;
	ConsecutiveShot++;

	LOG_FUNC_PRINTF(LogWeaponComponent, Log, "AmmoSpentInClip: %d, ConsecutiveShot: %d", AmmoSpentInClip, ConsecutiveShot);
}

void UC_Weapon::ReloadClip()
{
	if (GetRemainingAmmo() > 0)
	{
		AmmoSpent       += AmmoSpentInClip;
		LoadedAmmo = FMath::Clamp(GetRemainingAmmo(), 0, AmmoPerClip);
		AmmoSpentInClip = 0;
		
		bCanFire = GetRemainingAmmoInClip() > 0;
		bCanReload = GetRemainingAmmo() > 0;
	}
}

void UC_Weapon::HandleDummy()
{
	SetIsReplicated(!IsDummy());

	TScriptDelegate<FNotThreadSafeDelegateMode> AttackImplDelegate;
	AttackImplDelegate.BindUFunction(this, "AttackImplementation");
	TScriptDelegate<FNotThreadSafeDelegateMode> StopAttackImplDelegate;
	StopAttackImplDelegate.BindUFunction(this, "StopAttackImplementation");
	TScriptDelegate<FNotThreadSafeDelegateMode> ReloadImplDelegate;
	ReloadImplDelegate.BindUFunction(this, "ReloadImplementation");
	
	if (UC_Weapon* Sibling = GetSiblingComponent();
		Sibling && IsDummy())
	{
		Sibling->OnAttackStart.Add(AttackImplDelegate);
		Sibling->OnStopAttack.Add(StopAttackImplDelegate);
		Sibling->OnReloadStart.Add(ReloadImplDelegate);
	}
	
	{
		TScriptDelegate<FNotThreadSafeDelegateMode> SoundDelegate;
		SoundDelegate.BindUFunction(this, "Multi_PlayAttackSound");
		TScriptDelegate<FNotThreadSafeDelegateMode> AmmoDelegate;
		AmmoDelegate.BindUFunction(this, "ConsumeAmmo");

		if (!IsDummy())
		{
			OnAttackStart.Add(SoundDelegate);
			OnAttackStart.Add(AmmoDelegate);
		}
		else
		{
			OnAttackStart.Remove(SoundDelegate);
			OnAttackStart.Remove(AmmoDelegate);
		}
	}

	{
		TScriptDelegate<FNotThreadSafeDelegateMode> SoundDelegate;
		SoundDelegate.BindUFunction(this, "Multi_PlayReloadSound");
		TScriptDelegate<FNotThreadSafeDelegateMode> ReloadDelegate;
		ReloadDelegate.BindUFunction(this, "Reload");

		if (!IsDummy())
		{
			OnReloadStart.Add(SoundDelegate);
			OnReloadStart.Add(ReloadDelegate);
		}
		else
		{
			OnReloadStart.Remove(SoundDelegate);
			OnReloadStart.Remove(ReloadDelegate);
		}
	}
}

void UC_Weapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UC_Weapon , bFiring , COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UC_Weapon , bReloading , COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UC_Weapon , bCanReload , COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UC_Weapon , bCanFire , COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UC_Weapon , AmmoSpent , COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UC_Weapon , AmmoSpentInClip , COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UC_Weapon , LoadedAmmo , COND_OwnerOnly);
}


// Called every frame
void UC_Weapon::TickComponent(float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	// ...
}

