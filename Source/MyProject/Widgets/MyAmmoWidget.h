// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interfaces/MyPlayerStateRequiredWidget.h"

#include "MyProject/Components/C_PickUp.h"

#include "MyAmmoWidget.generated.h"

class AA_Collectable;
class AMyPlayerState;
class AMyCollectable;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyAmmoWidget : public UUserWidget, public IMyPlayerStateRequiredWidget
{
	GENERATED_BODY()

protected:
	virtual void DispatchPlayerState(AMyPlayerState* InPlayerState) override;
	
private:
	
	UFUNCTION()
	void UpdateAmmo(const int32 CurrentAmmoCount, const int32 RemainingAmmoCount);

	UFUNCTION()
	void HandleWeaponChanged(UC_PickUp* InPrevious, UC_PickUp* InNew, AMyPlayerState* InPlayerState);
	
	
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* AmmoText;
};
