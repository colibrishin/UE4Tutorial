// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mutex>

#include "CoreMinimal.h"
#include "Enum.h"
#include "Utilities.hpp"

#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateChanged, AMyPlayerState*, InPlayerState, const EMyCharacterState, InCurrentState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChanged, const float, InHP);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMoneyChanged, int32)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageTaken, AMyPlayerState*, InPlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHandChanged, AMyCollectable*, InPrevious, AMyCollectable*, InCurrent, AMyPlayerState*, InPlayerState);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnKillOccurred, AMyPlayerState*, AMyPlayerState*, const class AMyWeapon*)

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMyPlayerState();

	EMyTeam GetTeam() const
	{
		std::lock_guard<std::mutex> Lock(TeamAssignMutex);
		return Team;
	}
	void AssignTeam();

	class UMyStatComponent* GetStatComponent() const { return StatComponent; }
	class UMyInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	class AMyCollectable* GetCurrentHand() const { return CurrentHand; }

	void Use(const int32 Index);

	void SetState(const EMyCharacterState NewState);
	void SetHP(const int32 NewHP);
	void AddMoney(const int32 Amount);
	void SetCurrentWeapon(class AMyWeapon* NewWeapon);
	void SetCurrentItem(class AMyCollectable* NewItem);

	FORCEINLINE float GetHP() const
	{
		return Health;
	}
	FORCEINLINE float GetHPRatio() const;

	
	FOnDamageTaken OnDamageTaken;

	FOnStateChanged OnStateChanged;

	FOnHPChanged OnHPChanged;

	FOnHandChanged OnHandChanged;
	
	DECL_BINDON(OnMoneyChanged, int32)
	DECL_BINDON(OnKillOccurred, class AMyPlayerState*, class AMyPlayerState*, const class AMyWeapon*)

	FORCEINLINE int32 GetMoney() const { return Money; }

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	int32         GetDamage() const;
	EMyCharacterState GetState() const { return State; }

	virtual void Reset() override;
	void         IncrementKills();
	void         IncrementDeaths();

	int32 GetKill() const { return Kill; }
	int32 GetDeath() const { return Death; }
	int32 GetAssist()const { return Assist; }

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	
	UFUNCTION()
	void OnRep_HealthChanged() const;

	UFUNCTION()
	void OnRep_HandChanged(class AMyCollectable* PreviousHand);

	void SetTeam(const EMyTeam NewTeam)
	{
		std::lock_guard<std::mutex> Lock(TeamAssignMutex);
		Team = NewTeam;
	}

	UFUNCTION(Client, Reliable)
	void Client_OnDamageTaken(AMyPlayerState* DamageGiver);

	UFUNCTION()
	void OnRep_MoneyChanged() const;

	static std::mutex TeamAssignMutex;

	UPROPERTY(VisibleAnywhere, Replicated)
	EMyCharacterState State;

	UPROPERTY(VisibleAnywhere, Replicated)
	EMyTeam Team;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 Kill;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 Death;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 Assist;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_HealthChanged)
	int32 Health;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_MoneyChanged)
	int32 Money;

	UPROPERTY(VisibleAnywhere, Replicated)
	class UMyStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, Replicated)
	class UMyInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_HandChanged)
	class AMyCollectable* CurrentHand;

	FOnKillOccurred OnKillOccurred;

	FOnMoneyChanged OnMoneyChanged;

};
