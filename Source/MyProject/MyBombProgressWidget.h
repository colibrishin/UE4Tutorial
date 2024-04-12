// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyBombProgressWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBombProgressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetValue(const float Value) const;
	void BindBomb(const class AMyC4* NewBomb);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(Meta=(BindWidget))
	class UProgressBar* ProgressBar;

	TWeakObjectPtr<const AMyC4> Bomb;

};
