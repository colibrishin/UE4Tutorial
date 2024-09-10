// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MyPlayerState.h"

#include "Blueprint/UserWidget.h"
#include "../Interfaces/MyPlayerStateRequiredWidget.h"
#include "MyCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyCharacterWidget : public UUserWidget, public IMyPlayerStateRequiredWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void DispatchPlayerState(AMyPlayerState* InPlayerState) override;
	
private:
	UFUNCTION()
	void UpdateHpRatio(const float Value);

	UPROPERTY(Meta=(BindWidget))
	class UProgressBar* HPProgressBar;
};
