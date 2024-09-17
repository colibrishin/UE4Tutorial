// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_Health.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHealthComponent, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, const int32, InOld, const int32, InNew);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChangedRatio, const float, InRatio);

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_Health : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class UC_CharacterAsset;

	FOnHPChanged OnHPChanged;

	FOnHPChangedRatio OnHPChangedRatio;
	
	// Sets default values for this component's properties
	UC_Health();

	void Increase(const int32 InValue);
	void Decrease(const int32 InValue);
	
	int32 GetHealth() const { return Health; }
	int32 GetMaxHealth() const { return MaxHealth; }
	float GetHPRatio() const { return FMath::Clamp(static_cast<float>(Health / MaxHealth), 0.f, 1.f); }
	void  Reset()
	{
		if (GetNetMode() == NM_Client)
		{
			return;
		}

		Health = MaxHealth;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_HP(const int32 InOld) const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(VisibleAnywhere, meta=(ClampMin=0))
	int32 MaxHealth;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_HP, meta=(ClampMin=0))
	int32 Health;
};
