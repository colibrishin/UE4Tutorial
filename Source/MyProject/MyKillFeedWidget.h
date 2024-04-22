// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	void HandleKillOccurred(class AMyPlayerState* Killer, class AMyPlayerState* Victim, const class AMyWeapon* Weapon);

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
