// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCollectable.h"
#include "MyWeaponStatComponent.h"
#include "Utilities.hpp"

#include "Engine/DataTable.h"

#include "GameFramework/Actor.h"
#include "MyWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnFireReady)
DECLARE_MULTICAST_DELEGATE(FOnReloadReady)

UCLASS()
class MYPROJECT_API AMyWeapon : public AMyCollectable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyWeapon();

	DECL_BINDON(OnFireReady)
	DECL_BINDON(OnReloadReady)

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool AttackImpl() PURE_VIRTUAL(AMyWeapon::AttackImpl, return false;);
	virtual bool AttackInterruptedImpl();
	virtual bool ReloadImpl() PURE_VIRTUAL(AMyWeapon::ReloadImpl, return false;);

	virtual bool PostInteract(AMyCharacter* Character) override;

	virtual void OnFireRateTimed();
	virtual void OnReloadDone();
	virtual void OnCookingTimed();

	virtual void DropBeforeCharacter() override;

	UPROPERTY(VisibleAnywhere, Replicated)
	uint32 ConsecutiveShots;

	FTimerHandle FireRateTimerHandle;

	FTimerHandle ReloadTimerHandle;

	FTimerHandle CookingTimerHandle;

private:

	UPROPERTY(VisibleAnywhere)
	bool CanReload;

	UPROPERTY(VisibleAnywhere)
	bool CanAttack;

	UPROPERTY(VisibleAnywhere)
	bool bIsDummyVisually;

	UPROPERTY(EditAnywhere)
	class UTexture2D* WeaponImage;
	
	FOnFireReady OnFireReady;

	FOnReloadReady OnReloadReady;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UMyWeaponStatComponent* WeaponStatComponent;

};
