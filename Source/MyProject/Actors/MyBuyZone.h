// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Private/Enum.h"

#include "GameFramework/Actor.h"
#include "MyBuyZone.generated.h"

UCLASS()
class MYPROJECT_API AMyBuyZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyBuyZone();

	EMyTeam GetTeam() const { return Team; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	EMyTeam Team;

};
