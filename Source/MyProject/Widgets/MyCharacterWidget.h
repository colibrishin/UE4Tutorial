// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MyPlayerState.h"

#include "Blueprint/UserWidget.h"
#include "../Interfaces/CharacterRequiredWidget.h"
#include "MyCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyCharacterWidget : public UUserWidget , public ICharacterRequiredWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void DispatchCharacter(AA_Character* InCharacter) override;
	
private:
	UFUNCTION()
	void UpdateHpRatio(const float Value);

	UPROPERTY(Meta=(BindWidget))
	class UProgressBar* HPProgressBar;
};
