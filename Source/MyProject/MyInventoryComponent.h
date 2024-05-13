// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyInventoryComponent();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool TryAddItem(class AMyCollectable* Item);

	class AMyCollectable* Use(const int32 Index);

	template <typename T, typename CollectableLock = std::enable_if_t<std::is_base_of_v<AMyCollectable, T>>>
	T* Get() const
	{
		for (const auto& Item : Inventory)
		{
			const T* TypeCheck = Cast<T>(Item);

			if (TypeCheck)
			{
				return TypeCheck;
			}
		}

		return nullptr;
	}

	template <typename T , typename CollectableLock = std::enable_if_t<std::is_base_of_v<AMyCollectable , T>>>
	void Remove()
	{
		for (int32 i = 0; i < Inventory.Num(); ++i)
		{
			const T* TypeCheck = Cast<T>(Inventory[i]);

			if (TypeCheck)
			{
				Inventory.RemoveAt(i);
				return;
			}
		}
	}

	void Remove(AMyCollectable* MyCollectable);

	bool Find(AMyCollectable* MyCollectable) const;

	void Clear();
	void DropAll();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void HandleItemDestroy(AActor* ToBeDestroyed);

	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<TWeakObjectPtr<class AMyCollectable>> Inventory;

	UPROPERTY(Replicated, VisibleAnywhere)
	int32 InventorySize;

};
