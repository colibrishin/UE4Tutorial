// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interfaces/MyPlayerStateRequiredWidget.h"

#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Components/Weapon/C_Weapon.h"
#include "MyProject/Interfaces/CharacterRequiredWidget.h"

#include "MyAmmoWidget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAmmoWidget, Log, All);

class AA_Collectable;
class AMyPlayerState;
class AMyCollectable;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyAmmoWidget : public UUserWidget, public ICharacterRequiredWidget
{
	GENERATED_BODY()

protected:
	virtual void DispatchCharacter(AA_Character* InCharacter) override;
	
private:
	
	UFUNCTION()
	void UpdateAmmo(const int32 CurrentAmmoCount, const int32 RemainingAmmoCount, UC_Weapon* InWeapon);

	UFUNCTION()
	void HandleWeaponChanged(UChildActorComponent* InNew);
	
	
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* AmmoText;
};
