// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mutex>

#include "CoreMinimal.h"
#include "Enum.h"
#include "MyProjectGameModeBase.h"

#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"


/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMyPlayerState();

	EMyTeam GetTeam() const { return Team; }

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void AssignTeam();

	std::mutex TeamAssignMutex;

	UPROPERTY(VisibleAnywhere, Replicated)
	EMyTeam Team;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 Kill;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 Death;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 Assist;

};
