// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MyProject/Interfaces/EventHandler.h"
#include "SS_EventGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API USS_EventGameInstance : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	TScriptInterface<IEventHandler> AddEvent( const TSubclassOf<UObject>& Handler );

	UPROPERTY(VisibleAnywhere)
	TMap<TSubclassOf<UObject>, UObject*> Handlers;
};
