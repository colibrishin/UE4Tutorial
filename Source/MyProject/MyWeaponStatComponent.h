// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyWeaponStatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyWeaponStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMyWeaponStatComponent();
	int32 GetDamage() const { return Damage; }
	int32 GetID() const { return ID; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

private:
	UPROPERTY(EditAnywhere, Category=Stats, Meta=(AllowPrivateAccess))
	int32 ID;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	int32 Damage;

};
