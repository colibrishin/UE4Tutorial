// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_Jump.generated.h"

UCLASS(Blueprintable)
class MYPROJECT_API AGM_Jump : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGM_Jump();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	int32 CharacterAssetID;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
