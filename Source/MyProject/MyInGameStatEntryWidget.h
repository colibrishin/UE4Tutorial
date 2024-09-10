// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Private/Enum.h"
#include "Blueprint/UserWidget.h"
#include "MyInGameStatEntryWidget.generated.h"

class AMyPlayerState;
class UTextBlock;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyInGameStatEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayer(AMyPlayerState* PlayerState);

	AMyPlayerState*  GetPlayer() const { return DesignatedPlayerState.Get(); }

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void ForceUpdatePlayerStats() const;

	void HandleStateChanges(class AMyPlayerController* Controller, EMyTeam Team, EMyCharacterState State) const;

	float DeltaTime = 0.f;

	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AMyPlayerState> DesignatedPlayerState;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	UTextBlock* PlayerName;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	UTextBlock* KillCount;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	UTextBlock* DeathCount;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	UTextBlock* AssistCount;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	UTextBlock* PingValue;
};
