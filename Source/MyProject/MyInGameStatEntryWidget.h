// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"

#include "Blueprint/UserWidget.h"
#include "MyInGameStatEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyInGameStatEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayer(class AMyPlayerState* PlayerState);

	class AMyPlayerState*  GetPlayer() const { return DesignatedPlayerState.Get(); }

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void ForceUpdatePlayerStats() const;

	void HandleStateChanges(class AMyPlayerController* Controller, EMyTeam Team, EMyCharacterState State) const;

	float DeltaTime = 0.f;

	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<class AMyPlayerState> DesignatedPlayerState;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UTextBlock* PlayerName;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UTextBlock* KillCount;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UTextBlock* DeathCount;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UTextBlock* AssistCount;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UTextBlock* PingValue;
};
