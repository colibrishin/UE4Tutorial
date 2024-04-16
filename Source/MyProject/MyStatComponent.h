// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "Components/ActorComponent.h"
#include "MyStatComponent.generated.h"


struct FMyStat;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyStatComponent();

	FORCEINLINE int32 GetLevel() const { return Level; }
	FORCEINLINE int32 GetDamage() const { return Damage; }
	FORCEINLINE int32 GetMaxHealth() const { return MaxHealth; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Stats", Meta=(AllowPrivateAccess))
	int32 Level;

	UPROPERTY(VisibleAnywhere, Category = "Stats", Meta=(AllowPrivateAccess))
	int32 Damage;

	UPROPERTY(VisibleAnywhere, Category = "Stats", Meta=(AllowPrivateAccess))
	int32 MaxHealth;

};
