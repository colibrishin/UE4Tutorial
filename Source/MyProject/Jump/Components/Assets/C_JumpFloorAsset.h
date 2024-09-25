// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/Components/Asset/C_Asset.h"
#include "C_JumpFloorAsset.generated.h"


UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_JumpFloorAsset : public UC_Asset
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_JumpFloorAsset();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction
	) override;
	
	virtual void ApplyAsset() override;
};
