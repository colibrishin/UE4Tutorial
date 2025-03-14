// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mutex>

#include "CoreMinimal.h"

#include "Actors/BaseClass/A_Character.h"

#include "Components/C_PickUp.h"

#include "Private/Enum.h"

#include "GameFramework/PlayerState.h"

#include "MyPlayerState.generated.h"

class UC_Health;
class UC_PickUp;
class UC_Weapon;
class AMyCollectable;
class AMyPlayerController;
class UC_Buy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateChanged , AMyPlayerState* , InPlayerState ,
                                             const EMyCharacterState , InCurrentState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyChanged, const int32, InNewMoney);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageTaken, AMyPlayerState*, InPlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnKillOccurred, AMyPlayerController*, InKiller, AMyPlayerController*, InVictim, UC_PickUp*, InWeapon);

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerState, Log, All);
/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMyPlayerState();

	EMyTeam GetTeam() const { return Team; }
	void AssignTeam();

	void       SetState(const EMyCharacterState NewState);
	void       AddMoney(const int32 Amount);
	UC_Buy*    GetBuyComponent() const;
	int32      GetCharacterAssetID() const;

	FOnStateChanged OnStateChanged;

	FOnKillOccurred OnKillOccurred;
	
	FOnMoneyChanged OnMoneyChanged;
	
	FORCEINLINE int32 GetMoney() const { return Money; }

	EMyCharacterState GetState() const { return State; }

	virtual void Reset() override;
	void         IncrementKills();
	void         IncrementDeaths();

	int32 GetKill() const { return Kill; }

	int32 GetDeath() const { return Death; }

	int32 GetAssist() const { return Assist; }

protected:
	virtual void    BeginPlay() override;

	UFUNCTION()
	void UpdateCharacterAsset(APlayerState* InPlayerState, APawn* InOldCharacter, APawn* InNewCharacter);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	void SetTeam(const EMyTeam NewTeam) { Team = NewTeam; }

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

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_MoneyChanged)
	int32 Money;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess, ClampMin=0))
	int32 CharacterAssetID;
	
	UPROPERTY(VisibleAnywhere, Replicated, meta=(AllowPrivateAccess))
	UC_Buy* BuyComponent;

};
