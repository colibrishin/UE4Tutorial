// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "C_Weapon.generated.h"

class IPickableObject;
struct FEnhancedInputActionEventBinding;
class UInputAction;
class UInputMappingContext;
enum class EMyWeaponType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(
 FOnAmmoConsumed , const int32 , InOld , const int32 , InNew ,
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

	uint32 GetRemainingAmmo() const;

	uint32 GetRemainingAmmoInClip() const;

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

	UFUNCTION(Server , Reliable)
	void Server_Attack();

	UFUNCTION(Server , Reliable)
	void Server_Reload();

	bool ValidateAttack();

	bool ValidateReload();

	UFUNCTION()
	void OnRep_OnAmmoUpdated(const int32 InOld);

	UFUNCTION()
	void HandleAttackStart(UC_Weapon* InWeapon);

	UFUNCTION()
	void HandleAttackEnd(UC_Weapon* InWeapon);

	UFUNCTION()
	void HandleReloadStart(UC_Weapon* InWeapon);

	UFUNCTION()
	void HandleReloadEnd(UC_Weapon* InWeapon);

	UFUNCTION()
	virtual void HandlePickUp(TScriptInterface<IPickableObject> InPickUpObject);

	UFUNCTION()
	virtual void HandleDrop(TScriptInterface<IPickableObject> InPickUpObject);

	UFUNCTION(NetMulticast , Unreliable)
	void Multi_PlayAttackSound();

	UFUNCTION(NetMulticast , Unreliable)
	void Multi_PlayReloadSound();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere, Replicated)
	bool bFiring;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bCanReload;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bCanFire;

	UPROPERTY(VisibleAnywhere, Category=Stats, ReplicatedUsing=OnRep_OnAmmoUpdated)
	int32 AmmoSpent;

	UPROPERTY(VisibleAnywhere, Category=Stats, Replicated)
	int32 AmmoSpentInClip;
	
	UPROPERTY(VisibleAnywhere)
	bool bCanSpray;

	UPROPERTY(VisibleAnywhere)
	int32 Damage;

	UPROPERTY(VisibleAnywhere)
	float AttackRate;

	UPROPERTY(VisibleAnywhere)
	float Range;
	
	UPROPERTY(VisibleAnywhere)
	int32 ConsecutiveShot;
	
	UPROPERTY(VisibleAnywhere)
	int32 AmmoPerClip;

	UPROPERTY(VisibleAnywhere)
	int32 TotalAmmo;

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

public:
	// Called every frame
	virtual void TickComponent(
		float                        DeltaTime , ELevelTick TickType ,
		FActorComponentTickFunction* ThisTickFunction
	) override;
};
