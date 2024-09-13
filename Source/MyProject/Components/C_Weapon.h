// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "C_Weapon.generated.h"

struct FEnhancedInputActionEventBinding;
class UInputAction;
class UInputMappingContext;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAmmoConsumed , const int32 , InOld , const int32 , InNew ,
                                               UC_Weapon* , InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnSprayStarted , UC_Weapon* , InWeapon );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnSprayEnded , UC_Weapon* , InWeapon );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackStart, UC_Weapon*, InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackEnd, UC_Weapon*, InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadStart, UC_Weapon*, InWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadEnd, UC_Weapon*, InWeapon);
DECLARE_LOG_CATEGORY_EXTERN(LogWeaponComponent, Log, All);

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_Weapon : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_Weapon();

	FOnAmmoConsumed OnAmmoUpdated;

	FOnAttackStart OnAttackStart;

	FOnAttackEnd OnAttackEnd;

	FOnReloadStart OnReloadStart;

	FOnReloadEnd OnReloadEnd;

	int32 GetRemainingAmmo() const;

	int32 GetRemainingAmmoInClip() const;

	int32 GetAmmoPerClip() const;

	void Attack();

	void StopAttack();

	void Reload();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_StopAttack();

	UFUNCTION(Server, Reliable)
	void Server_Attack();

	UFUNCTION(Server, Reliable)
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
	void HandlePickUp( IPickableObject* InPickUpObject ) const;

	UFUNCTION()
	void HandleDrop( IPickableObject* InPickUpObject ) const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UPROPERTY(VisibleAnywhere, Replicated)
	bool bFiring;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bCanReload;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bCanFire;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bCanSpray;

	UPROPERTY(VisibleAnywhere, Category=Stats, ReplicatedUsing=OnRep_OnAmmoUpdated)
	int32 AmmoSpent;

	UPROPERTY(VisibleAnywhere, Category=Stats, Replicated)
	int32 AmmoSpentInClip;

	UPROPERTY(VisibleAnywhere, Category=Stats, Replicated)
	int32 AmmoPerClip;

	UPROPERTY(VisibleAnywhere, Category=Stats, Replicated)
	int32 TotalAmmoCount;

	UPROPERTY(EditAnywhere)
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere)
	UInputAction* AttackAction;

	UPROPERTY( EditAnywhere )
	UInputAction* ReloadAction;

	FEnhancedInputActionEventBinding* AttackStartBinding;

	FEnhancedInputActionEventBinding* AttackStopBinding;

	FEnhancedInputActionEventBinding* ReloadBinding;

	FTimerHandle SprayTimerHandle;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime , ELevelTick TickType ,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
