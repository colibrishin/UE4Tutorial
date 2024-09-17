// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MyProject/Interfaces/AssetFetchable.h"

#include "A_Collectable.generated.h"

class UC_PickUp;
class UC_Weapon;
class UC_CollectableAsset;

UCLASS()
class MYPROJECT_API AA_Collectable : public AActor, public IAssetFetchable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AA_Collectable();

	UC_CollectableAsset* GetAssetComponent() const { return AssetComponent; }
	UC_PickUp* GetPickUpComponent() const { return PickUpComponent; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, Replicated, meta=(AllowPrivateAccess))
	UC_CollectableAsset* AssetComponent;

	UPROPERTY(VisibleAnywhere, Replicated, meta=(AllowPrivateAccess))
	UC_PickUp* PickUpComponent;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
