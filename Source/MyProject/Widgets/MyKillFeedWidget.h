// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MyProject/MyPlayerState.h"
#include "MyProject/Components/Weapon/C_Weapon.h"

#include "MyKillFeedWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyKillFeedWidget : public UUserWidget
{
	static constexpr float KillFeedDuration = 5.f;

	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UFUNCTION()
	void HandleKillOccurred(AMyPlayerState* Killer, AMyPlayerState* Victim, UC_PickUp* Weapon);

	void AddKillFeed(const FText& KillerName, const FText& VictimName, class UTexture2D* WeaponImage);

	void PushKillFeed(class UMyKillFeedDetailWidget* Widget);

	void PopKillFeed() const;

	bool Empty() const;

	float DeltaTime = 0.f;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* KillFeedPanel;

	UPROPERTY(EditAnywhere, Meta=(AllowPrivateAccess))
	TSubclassOf<UUserWidget> EntryClass;
	
};
