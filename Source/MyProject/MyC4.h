// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyItem.h"
#include "Utilities.hpp"

#include "GameFramework/Actor.h"
#include "MyC4.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBombStateChanged, EMyBombState)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBombPicked, class AMyCharacter*)

UCLASS()
class MYPROJECT_API AMyC4 : public AMyItem
{
	GENERATED_BODY()
	
public:
	static constexpr float FullPlantingTime = 4.0f;
	static constexpr float FullDefusingTime = 10.0f;
	static constexpr float FullExplodingTime = 25.0f;

	// Sets default values for this actor's properties
	AMyC4();

	float GetPlantingRatio() const { return PlantingTime / FullPlantingTime; }
	float GetDefusingRatio() const { return DefusingTime / FullDefusingTime; }
	float GetElapsed() const { return Elapsed; }

	bool IsPlanting() const { return BombState == EMyBombState::Planting; }
	bool IsDefusing() const { return BombState == EMyBombState::Defusing; }
	bool IsPlanted() const { return BombState == EMyBombState::Planted; }
	bool IsDefused() const { return BombState == EMyBombState::Defused; }
	bool IsExploded() const { return BombState == EMyBombState::Exploded; }

	bool IsPlantable(const bool bCheckSpeed = true) const;
	bool IsDefusable(const bool bCheckSpeed = true) const;

	const AMyCharacter* GetDefusingCharacter() const { return DefusingCharacter.Get(); }


	void SetState(const EMyBombState NewState)
	{
		LOG_FUNC_PRINTF(LogTemp, Warning, "SetState: %s", *EnumToString(NewState));
		BombState = NewState;
		OnBombStateChanged.Broadcast(BombState);
	}

	void         PlantInterrupted();

	DECL_BINDON(OnBombStateChanged, EMyBombState)
	DECL_BINDON(OnBombPicked, class AMyCharacter*)

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Server_InteractInterrupted_Implementation() override;
	virtual void Server_UseInterrupted_Implementation() override;

	virtual void Server_Interact_Implementation(AMyCharacter* Character) override;
	virtual void Server_Use_Implementation(AMyCharacter* Character) override;

	UFUNCTION(Client, Reliable)
	void Client_TryPlanting(AMyCharacter* Character);

	UFUNCTION(Client, Reliable)
	void Client_TryDefusing(AMyCharacter* Character);

	void PresetPlant(AMyCharacter* Character);
	void PresetDefuse(AMyCharacter* Character);

	UFUNCTION(Client, Reliable)
	void Client_UnsetDefuse(AMyCharacter* Character);

	void         OnBombExplodedImpl();
	void         OnBombPlantedImpl();
	void         OnBombDefusedImpl();

	virtual void Destroyed() override;

	virtual bool PreInteract(AMyCharacter* Character) override;
	virtual bool PostInteract(AMyCharacter* Character) override;
	virtual bool TryAttachItem(AMyCharacter* Character) override;

	virtual bool PreUse(AMyCharacter* Character) override;
	virtual bool PostUse(AMyCharacter* Character) override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	bool TDefuseGuard(AMyCharacter* Character) const;
	bool CTPickPlantGuard(AMyCharacter* Character) const;

	void SetDefusing(AMyCharacter* Character);
	void SetPlanting(const bool NewPlanting);
	bool TryDefuse(class AMyCharacter* Character);
	void DefuseInterrupted();
	bool TryPlant(class AMyCharacter* Character);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_NotifyPicked(AMyCharacter* Character) const;

	UPROPERTY(VisibleAnywhere, Replicated)
	EMyBombState BombState;

	UPROPERTY(VisibleAnywhere, Replicated)
	float Elapsed;

	UPROPERTY(VisibleAnywhere, Replicated)
	float PlantingTime;

	UPROPERTY(VisibleAnywhere, Replicated)
	float DefusingTime;

	UPROPERTY(VisibleAnywhere, Replicated)
	TWeakObjectPtr<class AMyCharacter> DefusingCharacter;

	FDelegateHandle PlanterAttackHandle;

	FDelegateHandle DefuserAttackHandle;

	FDelegateHandle DefuserOnInteractInterruptedHandle;

	FOnBombStateChanged OnBombStateChanged;

	FOnBombPicked OnBombPicked;

	FTimerHandle OnBombDefusingHandle;

	FTimerHandle OnBombExplodedHandle;

	FTimerHandle OnBombPlantingHandle;

};
