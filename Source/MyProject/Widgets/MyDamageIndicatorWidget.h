// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "MyProject/Interfaces/MyPlayerStateRequiredWidget.h"
#include "MyDamageIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyDamageIndicatorWidget : public UUserWidget, public IMyPlayerStateRequiredWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void DispatchPlayerState(AMyPlayerState* InPlayerState) override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	bool bVisible = false;

	float DeltaTime = 0.f;

	UFUNCTION()
	void HandleDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* IndicatorImage;
};
