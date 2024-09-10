// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Private/Enum.h"

#include "Components/ActorComponent.h"
#include "MyCollectableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyCollectableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyCollectableComponent();

	EMySlotType GetSlotType() const { return SlotType; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(AllowPrivateAccess))
	int32 ID;

	UPROPERTY(VisibleAnywhere)
	EMySlotType SlotType;

	const struct FMyCollectableData* Stat;
		
};
