// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "GameFramework/GameMode.h"
#include "MyProjectGameModeBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnFreezeStarted)
DECLARE_MULTICAST_DELEGATE(FOnRoundStarted)
DECLARE_MULTICAST_DELEGATE(FOnRoundEnded)

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	static constexpr float MatchFreezeTime = 5.f;
	static constexpr float MatchRoundTime = 300.f;
	static constexpr float MatchRoundEndDelay = 5.f;

	AMyProjectGameModeBase();

	DECL_BINDON(OnFreezeStarted)
	DECL_BINDON(OnRoundStarted)
	DECL_BINDON(OnRoundEnded)

protected:
	void         NextRound();
	void         OnRoundTimeRanOut();
	void         OnFreezeEnded();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	FORCEINLINE void TransitTo
	(
		const EMyRoundProgress            NextProgress,
		const TMulticastDelegate<void()>& NextDelegate,
		void (AMyProjectGameModeBase::*   NextFunction)(),
		const float                       Delay,
		FTimerHandle&                     NextHandle
	);

	UPROPERTY(VisibleAnywhere)
	EMyRoundProgress RoundProgress;

	FOnFreezeStarted OnFreezeStarted;
	FOnRoundStarted OnRoundStarted;
	FOnRoundEnded OnRoundEnded;

	FTimerHandle* CurrentHandle;
	FTimerHandle FreezeTimerHandle;
	FTimerHandle RoundTimerHandle;
	FTimerHandle RoundEndTimerHandle;

	void GoToFreeze();
	void GoToRound();
	void GoToRoundEnd();

};
