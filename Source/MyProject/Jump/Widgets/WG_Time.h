// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyProject/Actors/BaseClass/A_Collectable.h"

#include "MyProject/Components/C_PickUp.h"

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

	virtual void NativeDestruct() override;

	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void ShowWinText(AA_Collectable* InNew);
	
protected:

	UFUNCTION()
	void StartTimer(const bool InValue);
	
	UFUNCTION()
	void UpdateTime();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ElapsedTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* WinText;

	FTimerHandle UpdateHandle;
	
};
