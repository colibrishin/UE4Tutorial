// Fill out your copyright notice in the Description page of Project Settings.


#include "DA_AssetBase.h"

#if WITH_EDITOR
void UDA_AssetBase::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	OnAssetPropertyChanged.Broadcast();
}
#endif
