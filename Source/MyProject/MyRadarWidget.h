// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyRadarWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyRadarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMyRadarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	class UTexture2D* EnemyTexture;

	UPROPERTY(VisibleAnywhere)
	FSlateBrush EnemyBrush;

	UPROPERTY(EditAnywhere)
	class UTexture2D* TeammateTexture;

	UPROPERTY(VisibleAnywhere)
	FSlateBrush TeammateBrush;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UCanvasPanel* RadarCanvas;

	UPROPERTY(VisibleAnywhere)
	TArray<class UImage*> RadarImages;
	
};
