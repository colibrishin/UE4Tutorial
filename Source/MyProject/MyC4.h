// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyItem.h"
#include "Utilities.hpp"

#include "GameFramework/Actor.h"
#include "MyC4.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBombStateChanged, EMyBombState)

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

	bool IsPlanting() const { return BombState == EMyBombState::Planting; }
	bool IsDefusing() const { return BombState == EMyBombState::Defusing; }
	bool IsPlanted() const { return BombState == EMyBombState::Planted; }
	bool IsDefused() const { return BombState == EMyBombState::Defused; }
	bool IsExploded() const { return BombState == EMyBombState::Exploded; }

	bool IsPlantable() const;
	bool IsDefusable() const;

	const AMyCharacter* GetDefusingCharacter() const { return DefusingCharacter.Get(); }


	void SetState(const EMyBombState NewState)
	{
		LOG_FUNC_PRINTF(LogTemp, Warning, "SetState: %s", *EnumToString(NewState));
		BombState = NewState;
		OnBombStateChanged.Broadcast(BombState);
	}

	virtual void InteractInterrupted() override;
	virtual void UseInterrupted() override;

	DECL_BINDON(OnBombStateChanged, EMyBombState)

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void ClientInteractImpl(AMyCharacter* Character) override;

	void         OnBombExplodedImpl();
	void         OnBombPlantedImpl();
	void         OnBombDefusedImpl();

	virtual void Destroyed() override;

	virtual bool PreInteract(AMyCharacter* Character) override;
	virtual bool PostInteract(AMyCharacter* Character) override;
	virtual bool TryAttachItem(const AMyCharacter* Character) override;

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
	bool TryPlant(class AMyCharacter* Character);

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

	FDelegateHandle DefuserOnInteractInterruptedHandle;

	FOnBombStateChanged OnBombStateChanged;

	FTimerHandle OnBombDefusingHandle;

	FTimerHandle OnBombExplodedHandle;

	FTimerHandle OnBombPlantingHandle;

};
