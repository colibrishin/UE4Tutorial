// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/Private/Enum.h"

#include "Blueprint/UserWidget.h"

#include "MyProject/Actors/BaseClass/A_Character.h"

#include "MyRoundTimeWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyRoundTimeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateTime() const;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void HandleBombProgressChanged(const EMyBombState InOldState, const EMyBombState InNewState, const AA_Character* InPlanter, const AA_Character*
	                               InDefuser);

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* RoundTimeText;
};
