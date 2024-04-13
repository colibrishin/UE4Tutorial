// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mutex>

#include "CoreMinimal.h"
#include "InputCoreTypes.h"

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
	void             Populate() const;
	void             Open();
	void             Close();
	void             Toggle();
	bool             Validate(const int32 ID, class AMyCharacter* const& Character) const;
	FORCEINLINE bool IsOpened() const { return IsOpen; }

private:
	void ProcessBuy(const int32 ID) const;

	UFUNCTION(Server, Reliable)
	void Server_RequestBuy(class AMyCharacter* Character, const int32 ID) const;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess))
	class UUniformGridPanel* WeaponGridPanel;

	UPROPERTY(EditAnywhere, Meta=(AllowPrivateAccess))
	TSubclassOf<UUserWidget> ItemWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta=(AllowPrivateAccess))
	bool IsOpen;

};
