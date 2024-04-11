// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyItem.h"

#include "GameFramework/Actor.h"
#include "MyC4.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBombPlanted)

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

	bool BeingPlanted() const { return IsPlanting; }
	bool BeingDefused() const { return IsDefusing; }

	bool  IsPlantable(OUT FHitResult& OutResult) const;
	bool  IsDefusable() const;

	virtual bool Interact(class AMyCharacter* Character) override;
	virtual bool Use(class AMyCharacter* Character) override;
	virtual void Recycle() override;

	DECL_BINDON(OnBombPlantedDelegate)

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void         OnBombTickingImpl();
	void         OnBombPlantedImpl();
	void         OnBombDefusedImpl();

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	void SetDefusing(const bool NewDefusing, class AMyCharacter* Character);
	void SetPlanting(const bool NewPlanting);
	bool TryDefuse(class AMyCharacter* Character);

	UPROPERTY(VisibleAnywhere)
	bool IsPlanted;

	UPROPERTY(VisibleAnywhere)
	bool IsPlanting;

	UPROPERTY(VisibleAnywhere)
	bool IsDefusing;

	UPROPERTY(VisibleAnywhere)
	bool IsExploded;

	UPROPERTY(VisibleAnywhere)
	bool IsDefused;

	UPROPERTY(VisibleAnywhere)
	float Elapsed;

	UPROPERTY(VisibleAnywhere)
	float PlantingTime;

	UPROPERTY(VisibleAnywhere)
	float DefusingTime;

	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<class AMyCharacter> DefusingCharacter;

	FOnBombPlanted OnBombPlantedDelegate;

	FTimerHandle OnBombDefusing;

	FTimerHandle OnBombTicking;

	FTimerHandle OnBombPlanted;

};
