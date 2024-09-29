#include "Kismet/GameplayStatics.h"

#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Components/Asset/C_Asset.h"
#include "MyProject/Interfaces/AssetFetchable.h"

#include "Utilities.hpp"


AA_Collectable* CloneChildActor(AA_Collectable* InObject)
{
	if (!InObject)
	{
		return nullptr;
	}

	UWorld* World = InObject->GetWorld();
	check(World);

	const FTransform Transform {FQuat::Identity, InObject->GetActorLocation(), FVector::OneVector};
	
	AA_Collectable* GeneratedObject = World->SpawnActorDeferred<AA_Collectable>(
		InObject->GetClass(),
		Transform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
		ESpawnActorScaleMethod::OverrideRootScale);

	GeneratedObject->GetAssetComponent()->SetID(InObject->GetComponentByClass<UC_Asset>()->GetID());
	GeneratedObject->FetchAsset();

	for (UActorComponent* OriginalComponent : InObject->GetComponents())
	{
		// todo: use tag? (5.4 introduced)
		UActorComponent* Destination = GeneratedObject->FindComponentByClass
		(
			OriginalComponent->GetClass()
		);

		Destination->ReinitializeProperties(OriginalComponent);
	}

	UGameplayStatics::FinishSpawningActor(GeneratedObject, Transform);
	return GeneratedObject;
}