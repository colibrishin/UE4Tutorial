// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/Private/Enum.h"

#include "Components/ActorComponent.h"
#include "MyProject/Widgets/MyAmmoWidget.h"
#include "MyCollectableComponent.generated.h"


class UMyCollectableDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyCollectableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyCollectableComponent();

	void SetID(const int32 InID);
	int32 GetID() const { return ID; }
	
	EMySlotType GetSlotType() const;

	virtual void ApplyAsset() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnRep_ID();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void UpdateAsset();
	
	UMyCollectableDataAsset* GetAsset() const { return DataAsset; }
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_ID, Meta=(AllowPrivateAccess))
	int32 ID;

	UPROPERTY(VisibleAnywhere)
	UMyCollectableDataAsset* DataAsset;
		
};
