// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameState.h"

#include "Blueprint/UserWidget.h"
#include "MyScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindGameState(AMyGameState* State);

private:
	void UpdateScore(const EMyTeam Team) const;

	UPROPERTY(Meta=(BindWidget))
	class UTextBlock* ScoreText;

};
