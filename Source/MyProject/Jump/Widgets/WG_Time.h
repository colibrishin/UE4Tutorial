// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WG_Time.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UWG_Time : public UUserWidget
{
	GENERATED_BODY()

public:
	UWG_Time(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	
protected:
	UFUNCTION()
	void UpdateTime();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ElapsedTime;

	FTimerHandle UpdateHandle;
	
};
