// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MyProject/Interfaces/AssetFetchable.h"

#include "A_JumpFloor.generated.h"

class USplineComponent;
class UC_JumpFloorAsset;
class UC_JumpCheckpoint;

UCLASS()
class MYPROJECT_API AA_JumpFloor : public AActor, public IAssetFetchable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AA_JumpFloor(const FObjectInitializer& ObjectInitializer);

	UStaticMeshComponent* GetMesh() const { return Mesh; }
	UStaticMeshComponent* GetCollisionVolumeMesh() const { return CollisionVolumeMesh; }

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* DummyRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CollisionVolumeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UC_JumpCheckpoint* CheckpointComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UC_JumpFloorAsset* AssetComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
