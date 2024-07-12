// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCollectable.h"
#include "MyWeaponStatComponent.h"
#include "Utilities.hpp"

#include "Engine/DataTable.h"

#include "GameFramework/Actor.h"
#include "MyWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadReady);

UCLASS()
class MYPROJECT_API AMyWeapon : public AMyCollectable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyWeapon();

	int32 GetDamage() const { return WeaponStatComponent->GetDamage(); }
	UMyWeaponStatComponent* GetWeaponStatComponent() const { return WeaponStatComponent; }

	bool CanBeReloaded() const { return CanReload; }
	bool CanDoAttack() const { return CanAttack; }

	bool Attack();
	bool AttackInterrupted();
	bool Reload();

	virtual bool TryAttachItem(AMyCharacter* Character) override;
	UTexture2D*  GetWeaponImage() const { return WeaponImage; }

	void SetVisualDummy(const bool NewDummy) { bIsDummyVisually = NewDummy; }
	bool IsDummyVisually() const { return bIsDummyVisually; }

	uint32 GetConsecutiveShots() const { return ConsecutiveShots; }

	virtual void OnFireRateTimed();
	virtual void OnReloadDone();
	virtual void OnCookingTimed();

	FOnFireReady OnFireReady;

	FOnReloadReady OnReloadReady;

	FTimerHandle OnFireReadyTimerHandle;

	FTimerHandle OnReloadDoneTimerHandle;

	FTimerHandle OnCookingTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool AttackImpl() PURE_VIRTUAL(AMyWeapon::AttackImpl, return false;);
	virtual bool AttackInterruptedImpl();
	virtual bool ReloadImpl() PURE_VIRTUAL(AMyWeapon::ReloadImpl, return false;);

	virtual bool PostInteract(AMyCharacter* Character) override;

	virtual void DropBeforeCharacter() override;

	UFUNCTION(Unreliable, Client)
	void Client_PlayAttackSound();
	virtual void Client_PlayAttackSound_Implementation();

	UFUNCTION(Unreliable, Client)
	void Client_PlayReloadSound();
	virtual void Client_PlayReloadSound_Implementation();

	UPROPERTY(VisibleAnywhere, Replicated)
	uint32 ConsecutiveShots;


	UPROPERTY(EditAnywhere, Category = "Weapon", meta=(AllowPrivateAccess = true))
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = "Weapon", meta=(AllowPrivateAccess = true))
	class USoundBase* ReloadSound;
private:

	UPROPERTY(VisibleAnywhere)
	bool CanReload;

	UPROPERTY(VisibleAnywhere)
	bool CanAttack;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bIsDummyVisually;

	UPROPERTY(EditAnywhere)
	class UTexture2D* WeaponImage;

	UPROPERTY(EditAnywhere, Category = "Weapon", Replicated)
	class UMyWeaponStatComponent* WeaponStatComponent;

};
