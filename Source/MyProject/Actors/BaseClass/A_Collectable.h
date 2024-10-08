// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyProject/Components/Asset/C_CollectableAsset.h"

#include "MyProject/Interfaces/AssetFetchable.h"
#include "A_Collectable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDummyFlagSet, AA_Collectable*, InPreviousDummy);

class UC_PickUp;
class UC_Weapon;
class UC_CollectableAsset;

UCLASS()
class MYPROJECT_API AA_Collectable : public AActor, public IAssetFetchable
{
	GENERATED_BODY()

public:
	static const FName AssetComponentName;
	
	// Sets default values for this actor's properties
	AA_Collectable(const FObjectInitializer& ObjectInitializer);

	FOnDummyFlagSet OnDummyFlagSet;

	template <typename T = UC_CollectableAsset> requires (std::is_base_of_v<UC_CollectableAsset, T>)
	T* GetAssetComponent() const
	{
		return Cast<T>(AssetComponent);
	}
	
	UC_PickUp*           GetPickUpComponent() const { return PickUpComponent; }
	USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }
	void                 SetDummy(const bool InFlag, AA_Collectable* InSibling);

	bool                 IsDummy() const { return bDummy; }
	AA_Collectable*      GetSibling() const { return Sibling; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Dummy(AA_Collectable* InPreviousDummy) const;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	USkeletalMeshComponent* SkeletalMeshComponent;
	
	UPROPERTY(VisibleAnywhere, Replicated, meta=(AllowPrivateAccess))
	UC_CollectableAsset* AssetComponent;

	UPROPERTY(VisibleAnywhere, Replicated, meta=(AllowPrivateAccess))
	UC_PickUp* PickUpComponent;
	
	UPROPERTY(VisibleAnywhere, Replicated, meta=(AllowPrivateAccess))
	bool bDummy;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Dummy, meta=(AllowPrivateAccess))
	AA_Collectable* Sibling;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
