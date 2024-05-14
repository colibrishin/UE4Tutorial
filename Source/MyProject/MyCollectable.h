// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyInteractiveActor.h"

#include "GameFramework/Actor.h"
#include "MyCollectable.generated.h"

UCLASS()
class MYPROJECT_API AMyCollectable : public AMyInteractiveActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCollectable();

	UMeshComponent* GetMesh() const { return MeshComponent.Get(); }
	UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
	USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }

	class UBoxComponent* GetCollider() const { return Collider; }
	class AMyCharacter* GetItemOwner() const;

	bool Drop();

	void Hide() const;
	void Show() const;

	void SetSkeletalMesh();
	void SetStaticMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual bool OnCharacterOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, class AMyCharacter* Character, const FHitResult& SweepResult
	);

	virtual void InteractImpl(class AMyCharacter* Character) override;
	virtual void UseImpl(class AMyCharacter* Character) override;

	virtual void InteractInterruptedImpl() override;
	virtual void UseInterruptedImpl() override;

	virtual bool PreInteract(class AMyCharacter* Character);
	virtual bool TryAttachItem(const AMyCharacter* Character);
	virtual bool PostInteract(class AMyCharacter* Character);

	virtual bool PreUse(class AMyCharacter* Character);
	virtual bool PostUse(class AMyCharacter* Character);

	virtual void DropImpl();

	bool IsBelongToCharacter() const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	virtual void OnCharacterOverlapImpl(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Collider;

	TWeakObjectPtr<UMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY(VisibleAnywhere)
    class UMyCollectableComponent* CollectableComponent;

	FDelegateHandle OnInteractInterruptedHandle;

	FDelegateHandle OnUseInterruptedHandle;

};
