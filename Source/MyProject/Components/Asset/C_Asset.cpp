// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Asset.h"

#include "MyProject/DataAsset/DA_AssetBase.h"
#include "MyProject/Private/Data.h"
#include "MyProject/Private/Utilities.hpp"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogAssetComponent);

// Sets default values for this component's properties
UC_Asset::UC_Asset()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UC_Asset::ApplyAsset()
{
	if (const AActor* Actor = GetOwner())
	{
		if (USkeletalMeshComponent* SkeletalMeshComponent = Actor->GetComponentByClass<USkeletalMeshComponent>())
		{
			SkeletalMeshComponent->SetSkeletalMesh(AssetData->GetSkeletalMesh());
		}
	}
}

// Called when the game starts
void UC_Asset::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UC_Asset::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UC_Asset, ID);
}

void UC_Asset::OnRep_ID()
{
	FetchAsset();
}

void UC_Asset::FetchAsset()
{
	LOG_FUNC_PRINTF(LogAssetComponent, Log, "Fetch the asset %d...", ID);
	if (const FBaseAssetRow* Data = GetRowData<FBaseAssetRow>(GetWorld(), ID))
	{
		check(Data->AssetToLink);
		AssetData = Data->AssetToLink;
	}
	else
	{
		LOG_FUNC_PRINTF(LogAssetComponent, Warning, "Unable to fetch the asset %d", ID);
	}
}

