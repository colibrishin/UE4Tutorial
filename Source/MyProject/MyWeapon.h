// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCollectable.h"
#include "MyWeaponStatComponent.h"

#include "Engine/DataTable.h"

#include "GameFramework/Actor.h"
#include "MyWeapon.generated.h"

UCLASS()
class MYPROJECT_API AMyWeapon : public AMyCollectable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyWeapon();

	int32 GetDamage() const { return WeaponStatComponent->GetDamage(); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual bool Interact(class AMyCharacter* Character) override;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UMyWeaponStatComponent* WeaponStatComponent;

};
