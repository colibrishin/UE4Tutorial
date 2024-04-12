// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyBuyMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBuyMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Populate() const;
	void Open() const;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess))
	class UUniformGridPanel* WeaponGridPanel;

private:
	UPROPERTY(EditAnywhere, Meta=(AllowPrivateAccess))
	TSubclassOf<UUserWidget> ItemWidgetClass;

};
