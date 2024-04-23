// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyInGameStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyInGameStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Open();
	void Close();

protected:
	virtual void NativeConstruct() override;
	
private:
	void AddNewPlayer(class AMyPlayerState* State) const;

	UPROPERTY(VisibleAnywhere)
	bool bVisible = false;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UVerticalBox* CTStats;
	
	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UVerticalBox* TStats;

	UPROPERTY(EditAnywhere, Meta=(AllowPrivateAccess))
	TSubclassOf<class UMyInGameStatEntryWidget> EntryClass;
};
