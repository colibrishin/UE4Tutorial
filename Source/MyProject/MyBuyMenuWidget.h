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
	void Populate() const;
	void Open();
	void Close();

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	void ProcessBuy(const float Price) const;

	UFUNCTION(Server, Reliable)
	void Server_RequestBuy(class AMyCharacter* Character, const float Price) const;


	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess))
	class UUniformGridPanel* WeaponGridPanel;

	UPROPERTY(EditAnywhere, Meta=(AllowPrivateAccess))
	TSubclassOf<UUserWidget> ItemWidgetClass;

	std::mutex Mutex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta=(AllowPrivateAccess))
	bool IsOpen;

};
