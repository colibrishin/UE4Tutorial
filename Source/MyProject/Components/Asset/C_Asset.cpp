// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Asset.h"

#include "Components/CapsuleComponent.h"

#include "MyProject/DataAsset/DA_AssetBase.h"
#include "MyProject/Frameworks/Subsystems/SS_World.h"
#include "MyProject/Interfaces/AssetFetchable.h"
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
	SetIsReplicatedByDefault(true);

#if WITH_EDITOR
	if (AssetData)
	{
		AssetData->OnAssetPropertyChanged.AddUObject(this, &UC_Asset::ApplyAsset);
	}
#endif
}


void UC_Asset::ApplyAsset()
{
	if (const AActor* Actor = GetOwner())
	{
		bool bCapsule = false;

		if (UCapsuleComponent* CapsuleComponent = Actor->GetComponentByClass<UCapsuleComponent>())
		{
			CapsuleComponent->SetWorldScale3D(AssetData->GetSize());
			bCapsule = true;
		}
		

		if (USkeletalMeshComponent* SkeletalMeshComponent = Actor->GetComponentByClass<USkeletalMeshComponent>())
		{
			SkeletalMeshComponent->SetSkeletalMesh(AssetData->GetSkeletalMesh());
			SkeletalMeshComponent->SetRelativeLocation(AssetData->GetMeshOffset());
			SkeletalMeshComponent->SetRelativeRotation(AssetData->GetMeshRotation());

			if (!bCapsule)
			{
				SkeletalMeshComponent->SetWorldScale3D(AssetData->GetSize());
			}
		}
	}
}

#if WITH_EDITOR
void UC_Asset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (const FName PropertyName = PropertyChangedEvent.Property->GetFName();
		PropertyName == GET_MEMBER_NAME_CHECKED(UC_Asset, ID) &&
		PropertyChangedEvent.ChangeType == EPropertyChangeType::ValueSet)
	{
		FetchAsset();
		ApplyAsset();
	}
}
#endif

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
	OnAssetIDSet.Broadcast();
}

void UC_Asset::FetchAsset()
{
	LOG_FUNC_PRINTF(LogAssetComponent, Log, "Fetch the asset %d...", ID);

	const UWorld* World = GetWorld();

#if WITH_EDITOR
	if (GIsEditor)
	{
		World = GEditor->GetEditorWorldContext().World();
	}

	if (AssetData)
	{
		AssetData->OnAssetPropertyChanged.RemoveAll(this);
	}
#endif
	
	if (const FBaseAssetRow* Data = World->GetSubsystem<USS_World>()->GetRowData<FBaseAssetRow>(ID))
	{
		check(Data->AssetToLink);
		AssetData = Data->AssetToLink;
#if WITH_EDITOR
		AssetData->OnAssetPropertyChanged.AddUObject(this, &UC_Asset::ApplyAsset);
#endif
	}
	else
	{
		LOG_FUNC_PRINTF(LogAssetComponent, Warning, "Unable to fetch the asset %d", ID);
	}
}

