// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCollectable.h"
#include "MyWeaponStatComponent.h"

#include "../../Intermediate/ProjectFiles/Utilities.hpp"

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

	virtual bool OnCharacterOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32                OtherBodyIndex,
		bool                 bFromSweep, AMyCharacter* Character, const FHitResult& SweepResult
	) override;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UMyWeaponStatComponent* WeaponStatComponent;

};
