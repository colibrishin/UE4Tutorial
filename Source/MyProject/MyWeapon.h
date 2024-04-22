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

	virtual bool Attack() final;
	virtual bool Reload() final;

	virtual bool TryAttachItem(const AMyCharacter* Character) override;
	UTexture2D*  GetWeaponImage() const { return WeaponImage; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual bool AttackImpl() PURE_VIRTUAL(AMyWeapon::AttackImpl, return false;);
	virtual bool ReloadImpl() PURE_VIRTUAL(AMyWeapon::ReloadImpl, return false;);

	virtual bool PostInteract(AMyCharacter* Character) override;

	virtual void OnFireRateTimed();
	virtual void OnReloadDone();

private:
	UPROPERTY(VisibleAnywhere)
	bool CanReload;

	UPROPERTY(VisibleAnywhere)
	bool CanAttack;

	UPROPERTY(EditAnywhere)
	class UTexture2D* WeaponImage;

	FTimerHandle FireRateTimerHandle;

	FTimerHandle ReloadTimerHandle;
	
	FOnFireReady OnFireReady;

	FOnReloadReady OnReloadReady;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UMyWeaponStatComponent* WeaponStatComponent;

};
