// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "A_Collectable.h"

#include "MyProject/Interfaces/AttackObject.h"
#include "MyProject/Interfaces/ReloadObject.h"

#include "A_Weapon.generated.h"

class UNiagaraComponent;

UCLASS()
class MYPROJECT_API AA_Weapon : public AA_Collectable, public IAttackObject, public IReloadObject
{
	GENERATED_BODY()

public:
	using PairAssetComponentType = UC_Weapon;
	
	friend class UC_WeaponAsset;
	static const FName WeaponComponentName;
	
	// Sets default values for this actor's properties
	AA_Weapon(const FObjectInitializer& ObjectInitializer);

	UC_Weapon* GetWeaponComponent() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, Replicated, meta=(AllowPrivateAccess))
	UC_Weapon* WeaponComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Attack() override;

	virtual void Reload() override;
	
};
