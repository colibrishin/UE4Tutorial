// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HD_Jump.generated.h"

class UWG_Time;

UCLASS()
class MYPROJECT_API AHD_Jump : public AHUD
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHD_Jump();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWG_Time* ElapsedTimerWidget;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
