// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyProject/Components/C_PickUp.h"

#include "C_Weapon.generated.h"

class IPickingUp;
struct FEnhancedInputActionEventBinding;
class UInputAction;
class UInputMappingContext;
enum class EMyWeaponType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(
 FOnAmmoConsumed , const int32 , InCurrentClip , const int32 , InRemainingAmmo ,
 UC_Weapon* , InWeapon
);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSprayStarted , UC_Weapon* , InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSprayEnded , UC_Weapon* , InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackStart , UC_Weapon* , InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackEnd , UC_Weapon* , InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadStart , UC_Weapon* , InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadEnd , UC_Weapon* , InWeapon);

DECLARE_LOG_CATEGORY_EXTERN(LogWeaponComponent , Log , All);

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_Weapon : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class UC_WeaponAsset;
	
	// Sets default values for this component's properties
	UC_Weapon();

	FOnAmmoConsumed OnAmmoUpdated;

	FOnAttackStart OnAttackStart;

	FOnAttackEnd OnAttackEnd;

	FOnReloadStart OnReloadStart;

	FOnReloadEnd OnReloadEnd;

	EMyWeaponType GetWeaponType() const { return WeaponType; }
	
	uint32 GetRemainingAmmo() const;

	uint32 GetRemainingAmmoInClip() const;

	uint32 GetRemainingAmmoWithoutCurrentClip() const;

	uint32 GetAmmoPerClip() const;

	uint32 GetConsecutiveShot() const;

	void Attack();

	void StopAttack();

	void Reload();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(Server , Reliable)
	void Server_StopAttack();

	void StopAttackImplementation();

	UFUNCTION(Server , Reliable)
	void Server_Attack();

	void AttackImplementation();

	UFUNCTION(Server , Reliable)
	void Server_Reload();

	void ReloadImplementation();

	void ReloadClip();

	UFUNCTION(Client, Reliable)
	void Client_SetupPickupInput(const AA_Character* InCharacter);

	UFUNCTION(Client, Reliable)
	void Client_SetupDropInput(const AA_Character* InCharacter);

	bool ValidateAttack();

	bool ValidateReload();

	UFUNCTION()
	void OnRep_OnAmmoUpdated();

	UFUNCTION()
	void HandleAttackStart(UC_Weapon* InWeapon);

	UFUNCTION()
	void HandleAttackEnd(UC_Weapon* InWeapon);

	UFUNCTION()
	void HandleReloadStart(UC_Weapon* InWeapon);

	UFUNCTION()
	void HandleReloadEnd(UC_Weapon* InWeapon);

	UFUNCTION()
	virtual void HandlePickUp(TScriptInterface<IPickingUp> InPickUpObject);

	UFUNCTION()
	virtual void HandleDrop(TScriptInterface<IPickingUp> InPickUpObject);

	UFUNCTION(NetMulticast , Unreliable)
	void Multi_PlayAttackSound();

	UFUNCTION(NetMulticast , Unreliable)
	void Multi_PlayReloadSound();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere)
	EMyWeaponType WeaponType;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	bool bFiring;

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
	
	UPROPERTY(VisibleAnywhere)
	int32 AmmoPerClip;

	UPROPERTY(VisibleAnywhere)
	int32 TotalAmmo;
	
	UPROPERTY(VisibleAnywhere)
	bool bCanSpray;

	UPROPERTY(VisibleAnywhere)
	int32 Damage;

	UPROPERTY(VisibleAnywhere)
	float AttackRate;

	UPROPERTY(VisibleAnywhere)
	float ReloadTime;

	UPROPERTY(VisibleAnywhere)
	float Range;
	
	UPROPERTY(VisibleAnywhere)
	int32 ConsecutiveShot;

	UPROPERTY(VisibleAnywhere)
	USoundBase* AttackSound;

	UPROPERTY(VisibleAnywhere)
	USoundBase* ReloadSound;

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

public:
	// Called every frame
	virtual void TickComponent(
		float                        DeltaTime , ELevelTick TickType ,
		FActorComponentTickFunction* ThisTickFunction
	) override;
};
