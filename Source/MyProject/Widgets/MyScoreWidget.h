// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/MyGameState.h"

#include "Blueprint/UserWidget.h"
#include "MyScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyScoreWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	void BindGameState(AMyGameState* State);

	UFUNCTION()
	void UpdateScore(const EMyTeam Team);

	UPROPERTY(Meta=(BindWidget))
	class UTextBlock* ScoreText;

};
