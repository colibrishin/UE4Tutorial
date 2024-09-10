// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/MyPlayerStateRequiredWidget.h"
#include "MyAmmoWidget.generated.h"

class AMyPlayerState;
class AMyCollectable;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyAmmoWidget : public UUserWidget, public IMyPlayerStateRequiredWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

protected:
	virtual void DispatchPlayerState(AMyPlayerState* InPlayerState) override;
	
private:
	
	UFUNCTION()
	void UpdateAmmo(const int32 CurrentAmmoCount, const int32 RemainingAmmoCount);

	UFUNCTION()
	void HandleWeaponChanged(AMyCollectable* InPrevious, AMyCollectable* InNew, AMyPlayerState* InPlayerState);
	
	
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* AmmoText;
};
