// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Components/C_PickUp.h"

#include "C_Weapon.generated.h"

class AA_Character;
class IPickingUp;
struct FEnhancedInputActionEventBinding;
class UInputAction;
class UInputMappingContext;
class UDA_Weapon;

enum class EMyWeaponType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(
 FOnAmmoConsumed , const int32 , InCurrentClip , const int32 , InRemainingAmmo ,
 UC_Weapon* , InWeapon
);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSprayStarted , UC_Weapon* , InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSprayEnded , UC_Weapon* , InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackStart , UC_Weapon* , InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackEnd , UC_Weapon* , InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStopAttack, UC_Weapon*, InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadStart , UC_Weapon* , InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadEnd , UC_Weapon* , InWeapon);

DECLARE_LOG_CATEGORY_EXTERN(LogWeaponComponent , Log , All);

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_Weapon : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class UC_WeaponAsset;
	friend struct FWeaponInputComponentControl;
	
	// Sets default values for this component's properties
	UC_Weapon();

	FOnAmmoConsumed OnAmmoUpdated;

	FOnAttackStart OnAttackStart;

	FOnAttack OnAttack;

	FOnAttackEnd OnAttackEnd;

	FOnStopAttack OnStopAttack;

	FOnReloadStart OnReloadStart;

	FOnReload OnReload;

	FOnReloadEnd OnReloadEnd;

	EMyWeaponType GetWeaponType() const;

	float GetRange() const;
	
	uint32 GetRemainingAmmo() const;

	uint32 GetRemainingAmmoInClip() const;

	uint32 GetRemainingAmmoWithoutCurrentClip() const;

	uint32 GetAmmoPerClip() const;

	uint32 GetConsecutiveShot() const;

	uint32 GetDamage() const;

	void Attack();

	void StopAttack();

	void Reload();

	bool IsDummy() const;

	UC_Weapon* GetSiblingComponent() const;

	void UpdateFrom( const UC_Weapon* InOtherComponent );

	virtual void UpdateFrom( UDA_Weapon* InAsset );

	UFUNCTION()
	virtual void HandlePickUp( TScriptInterface<IPickingUp> InPickUpObject , const bool bCallPickUp );

	UFUNCTION()
	virtual void HandleDrop( TScriptInterface<IPickingUp> InPickUpObject , const bool bCallDrop );

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	UFUNCTION(Server , Reliable)
	void Server_StopAttack();

	UFUNCTION(Server , Reliable)
	void Server_Attack();
	
	UFUNCTION(Server , Reliable)
	void Server_Reload();

	UFUNCTION(Client, Reliable)
	void Client_SetupPickupInput(const AA_Character* InCharacter);

	UFUNCTION(Client, Reliable)
	void Client_SetupDropInput(const AA_Character* InCharacter);

	void SetupPickupInputImplementation( const AA_Character* InCharacter );

	void SetupDropInputImplementation( const AA_Character* InCharacter ) const;

	UFUNCTION(Client, Reliable)
	void Client_OnAttack();

	UFUNCTION(Client, Reliable)
	void Client_OnReload();

	UFUNCTION()
	void OnRep_OnAmmoUpdated();

	void OnAmmoUpdatedImplementation();

	UFUNCTION(NetMulticast , Unreliable)
	void Multi_PlayAttackSound();

	UFUNCTION(NetMulticast , Unreliable)
	void Multi_PlayReloadSound();

protected:
	void StopAttackImplementation();

	void AttackImplementation();

	void ReloadImplementation();

protected:
	bool ValidateAttack();

	bool ValidateReload();

protected:
	UFUNCTION()
	void HandleAttackStart(UC_Weapon* InWeapon);

	UFUNCTION()
	void HandleAttackEnd(UC_Weapon* InWeapon);
	
	UFUNCTION()
	void HandleStopAttack(UC_Weapon* InWeapon);

	UFUNCTION()
	void HandleDeferredStopAttack( UC_Weapon* InWeapon );

	UFUNCTION()
	void HandleReloadStart(UC_Weapon* InWeapon);

	UFUNCTION()
	void HandleReloadEnd(UC_Weapon* InWeapon);

	UFUNCTION()
	void ConsumeAmmo();

	UFUNCTION()
	void ReloadClip();

	UFUNCTION()
	void HandleDummy(AA_Collectable* InPreviousDummy);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void MoveAmmoInfo( AActor* InActor );

protected:

	void RefreshFireAvailability();

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bRequestFiring;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bFiring;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bReloading;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bCanReload;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bCanFire;

	UPROPERTY(VisibleAnywhere, Category=Stats, ReplicatedUsing=OnRep_OnAmmoUpdated)
	int32 AmmoSpent;

	UPROPERTY(VisibleAnywhere, Category=Stats, ReplicatedUsing=OnRep_OnAmmoUpdated)
	int32 AmmoSpentInClip;

	UPROPERTY(VisibleAnywhere, Category=Stats, Replicated)
	int32 LoadedAmmo;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	int32 TotalAmmo;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	int32 ConsecutiveShot;

	UPROPERTY(VisibleAnywhere, Category="Asset")
	UDA_Weapon* ReferenceAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* ReloadAction;

	FEnhancedInputActionEventBinding* AttackStartBinding;

	FEnhancedInputActionEventBinding* AttackStopBinding;

	FEnhancedInputActionEventBinding* ReloadBinding;

	FTimerHandle SprayTimerHandle;

	FTimerHandle ReloadTimerHandle;

	FTimerHandle DeferredAttackTimerHandle;

	FTimerHandle DeferredStopAttackTimerHandle;

public:
	// Called every frame
	virtual void TickComponent(
		float                        DeltaTime , ELevelTick TickType ,
		FActorComponentTickFunction* ThisTickFunction
	) override;
};
