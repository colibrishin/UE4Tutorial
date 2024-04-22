// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"

#include "Blueprint/UserWidget.h"
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
	void HandleBombProgressChanged(const EMyBombState State) const;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* RoundTimeText;
};
