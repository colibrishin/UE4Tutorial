// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GS_Jump.generated.h"

class UC_PickUp;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoinGained , UC_PickUp*, InPrevious, UC_PickUp*, InNew);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStarted, const bool, InValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEnded, const bool, InValue);

class UC_JumpCheckpoint;

UCLASS(Blueprintable)
class MYPROJECT_API AGS_Jump : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnGameStarted OnGameStarted;

	UPROPERTY(BlueprintAssignable)
	FOnCoinGained OnCoinGained;
	
	// Sets default values for this actor's properties
	AGS_Jump();

	void SetLastCheckPoint(UC_JumpCheckpoint* InCheckPoint) { LastCheckpoint = InCheckPoint; }

	UFUNCTION(BlueprintCallable)
	void SetStartTime();

	UFUNCTION(BlueprintCallable)
	void SetStopTime();

	float GetStartTime() const;
	
	UC_JumpCheckpoint* GetLastCheckpoint() const { return LastCheckpoint; }

protected:
	UFUNCTION()
	void ProcessWin(UC_PickUp* InPrevious, UC_PickUp* InNew);

	UFUNCTION()
	void ChangeLevel() const;

	UFUNCTION(BlueprintCallable)
	void OnStartedSet(const bool InValue);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UC_JumpCheckpoint* LastCheckpoint;

	UPROPERTY(VisibleAnywhere)
	float GameStartedTime;

	UPROPERTY(VisibleAnywhere)
	float GameEndedTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter=OnStartedSet)
	bool bStarted;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FTimerHandle WinDelay;
};
