#include "Kismet/GameplayStatics.h"

#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Components/Asset/C_Asset.h"
#include "MyProject/Interfaces/AssetFetchable.h"

#include "Utilities.hpp"


AA_Collectable* CloneChildActor(AA_Collectable* InObject, const std::function<void(AActor*)>& InDeferredFunction)
{
	if (!InObject)
	{
		return nullptr;
	}

	UWorld* World = InObject->GetWorld();
	check(World);

	const auto& PreSpawn = [&InObject, &InDeferredFunction](AActor* InActor)
	{
		AA_Collectable* GeneratedObject = Cast<AA_Collectable>(InActor);
		
		for (UActorComponent* OriginalComponent : InObject->GetComponents())
		{
			// todo: use tag? (5.4 introduced)
			UActorComponent* Destination = InActor->FindComponentByClass
			(
				OriginalComponent->GetClass()
			);

			Destination->ReinitializeProperties(OriginalComponent);
		}

		InDeferredFunction(InActor);
		GeneratedObject->GetAssetComponent()->SetID(InObject->GetAssetComponent()->GetID());
	};
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.CustomPreSpawnInitalization = PreSpawn;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.bNoFail = true;
	
	AA_Collectable* GeneratedObject = World->SpawnActor<AA_Collectable>(
		InObject->GetClass(),
		SpawnParameters);
		
	return GeneratedObject;
}