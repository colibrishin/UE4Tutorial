// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "MyRemainingTeamWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyRemainingTeamWidget : public UUserWidget
{
	GENERATED_BODY()

public:

protected:
	virtual void NativeConstruct() override;

private:
	void BindGameState(class AMyGameState* State) const;

	void HandlePlayerChanges(const EMyTeam Team, const int32 Count) const;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RemainingPlayersText;
	
};
