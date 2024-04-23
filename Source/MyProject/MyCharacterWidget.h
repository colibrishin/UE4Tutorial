// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerState.h"

#include "Blueprint/UserWidget.h"
#include "MyCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void         BindPlayerState(AMyPlayerState* MyPlayerState);

protected:
	virtual void NativeConstruct() override;
	
private:
	void UpdateHpRatio(const float Value) const;

	UPROPERTY(Meta=(BindWidget))
	class UProgressBar* HPProgressBar;
};
