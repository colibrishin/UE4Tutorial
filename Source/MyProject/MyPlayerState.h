// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mutex>

#include "CoreMinimal.h"
#include "Enum.h"
#include "MyProjectGameModeBase.h"

#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnStateChanged, class AMyPlayerController*, EMyTeam, EMyCharacterState)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, int32, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMoneyChanged, int32)

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
	class AMyWeapon* GetWeapon() const { return Weapon; }
	class AMyCollectable* GetCurrentItem() const { return CurrentItem; }

	void Use(const int32 Index);

	void SetState(const EMyCharacterState NewState);
	void SetHP(const int32 NewHP);
	void AddMoney(const int32 Amount);
	void SetWeapon(class AMyWeapon* NewWeapon);

	FORCEINLINE float GetHP() const
	{
		return Health;
	}
	FORCEINLINE float GetHPRatio() const;

	DECL_BINDON(OnHPChanged, int32, float)
	DECL_BINDON(OnMoneyChanged, int32)
	DECL_BINDON(OnStateChanged, class AMyPlayerController*, EMyTeam, EMyCharacterState)

	FORCEINLINE int32 GetMoney() const { return Money; }

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	int32         GetDamage() const;
	EMyCharacterState GetState() const { return State; }

	virtual void Reset() override;
protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_StateChanged() const;

	UFUNCTION()
	void OnRep_HealthChanged() const;

	void SetTeam(const EMyTeam NewTeam)
	{
		std::lock_guard<std::mutex> Lock(TeamAssignMutex);
		Team = NewTeam;
	}

	UFUNCTION()
	void OnRep_MoneyChanged() const;

	static std::mutex TeamAssignMutex;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_StateChanged)
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

	UPROPERTY(Replicated, VisibleAnywhere)
	class AMyWeapon* Weapon;

	UPROPERTY(Replicated, VisibleAnywhere)
	class AMyCollectable* CurrentItem;

	FOnStateChanged OnStateChanged;

	FOnHPChanged OnHPChanged;

	FOnMoneyChanged OnMoneyChanged;

};
