// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "Components/ActorComponent.h"
#include "MyStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChanged, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMoneyChanged, int32)


struct FMyStat;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyStatComponent();

	DECL_BINDON(OnHPChanged, float)
	DECL_BINDON(OnMoneyChanged, int32)

	FORCEINLINE uint32 GetLevel() const { return Level; }
	FORCEINLINE uint32 GetDamage() const { return Damage; }
	FORCEINLINE uint32 GetHealth() const { return Health; }
	FORCEINLINE uint32 GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetHPRatio() const
	{
		return FMath::Clamp((float)Health / (float)MaxHealth, 0.f, 1.f);
	}

	FORCEINLINE void SetHP(const int32 NewHP)
	{
		Health = FMath::Clamp(NewHP, 0, MaxHealth);
		OnHPChanged.Broadcast(GetHPRatio());
	}

	void AddMoney(const int32 MoneyAmount);

	FORCEINLINE void OnDamage(const int32 DamageAmount)
	{
		SetHP(Health - DamageAmount);
	}

	FORCEINLINE int32 GetMoney() const
	{
		return Money;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_MoneyChanged() const;


	UPROPERTY(EditAnywhere, Category = "Stats", Meta=(AllowPrivateAccess))
	int32 Level;

	UPROPERTY(VisibleAnywhere, Category = "Stats", Meta=(AllowPrivateAccess))
	int32 Damage;

	// todo: replication?
	UPROPERTY(VisibleAnywhere, Category = "Stats", Meta=(AllowPrivateAccess))
	int32 Health;

	UPROPERTY(VisibleAnywhere, Category = "Stats", Meta=(AllowPrivateAccess))
	int32 MaxHealth;

	UPROPERTY(ReplicatedUsing=OnRep_MoneyChanged, VisibleAnywhere, Category = "Stats", Meta=(AllowPrivateAccess))
	int32 Money;

	FOnHPChanged OnHPChanged;

	FOnMoneyChanged OnMoneyChanged;

};
