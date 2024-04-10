// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCollectable.h"

#include "GameFramework/Actor.h"
#include "MyC4.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBombPlanted)

UCLASS()
class MYPROJECT_API AMyC4 : public AMyCollectable
{
	GENERATED_BODY()
	
public:
	static constexpr float FullPlantingTime = 4.0f;
	static constexpr float FullDefusingTime = 10.0f;
	static constexpr float FullExplodingTime = 25.0f;

	// Sets default values for this actor's properties
	AMyC4();

	float GetPlantingRatio() const { return PlantingTime / FullPlantingTime; }

	DECL_BINDON(OnBombPlantedDelegate)

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void         OnBombTickingImpl();
	void         OnBombPlantedImpl();
	void         OnBombDefusedImpl();
	virtual bool UseImpl(class AMyCharacter* Character) override;

	virtual void Tick(float DeltaSeconds) override;

private:
	void SetDefusingCharacter(class AMyCharacter* Character);

	UPROPERTY(VisibleAnywhere)
	bool IsPlanted;

	UPROPERTY(VisibleAnywhere)
	bool IsPlanting;

	UPROPERTY(VisibleAnywhere)
	bool IsDefusing;

	UPROPERTY(VisibleAnywhere)
	bool IsExploded;

	UPROPERTY(VisibleAnywhere)
	float PlantingTime;

	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<class AMyCharacter> DefusingCharacter;

	FOnBombPlanted OnBombPlantedDelegate;

	FTimerHandle OnBombDefusing;

	FTimerHandle OnBombTicking;

	FTimerHandle OnBombPlanted;

};
