// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "MyProject/MyPlayerState.h"

#include "Blueprint/UserWidget.h"

#include "MyProject/Interfaces/MyPlayerStateRequiredWidget.h"

#include "MyBuyMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBuyMenuWidget : public UUserWidget, public IMyPlayerStateRequiredWidget
{
	GENERATED_BODY()

public:
	void             Populate();
	void             Open();
	void             Close();
	void             Toggle();

	FORCEINLINE bool IsOpened() const { return IsOpen; }

	UFUNCTION()
	void             BuyTimeEnded(bool NewBuyTime);
	
	virtual void     DispatchPlayerState(AMyPlayerState* InPlayerState) override;

protected:
	virtual void NativeConstruct() override;

private:
	void ProcessBuy(const int32 ID) const;

	UFUNCTION()
	void UpdateMoney(const int32 Money);

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess))
	class UUniformGridPanel* WeaponGridPanel;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess))
	class UTextBlock* CurrentMoney;

	UPROPERTY(EditAnywhere, Meta=(AllowPrivateAccess))
	TSubclassOf<UUserWidget> ItemWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta=(AllowPrivateAccess))
	bool IsOpen;

};
