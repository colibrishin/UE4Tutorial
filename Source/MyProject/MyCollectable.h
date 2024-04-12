// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCharacter.h"
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

	UStaticMeshComponent* GetMesh() const { return Mesh; }
	class UBoxComponent* GetCollider() const { return Collider; }
	class AMyCharacter* GetItemOwner() const { return ItemOwner.Get(); }

	virtual bool Interact(class AMyCharacter* Character) override;
	virtual bool Use(class AMyCharacter* Character) override;

	virtual void InteractInterrupted() override;
	virtual void UseInterrupted() override;

	virtual bool Drop();

	void Hide() const;
	void Show() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual bool OnCharacterOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, class AMyCharacter* Character, const FHitResult& SweepResult
	);

	bool IsBelongToCharacter() const;

	void SetItemOwner(class AMyCharacter* NewOwner);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	virtual void OnCharacterOverlapImpl(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Collider;

	UPROPERTY(Replicated, VisibleAnywhere)
	TWeakObjectPtr<class AMyCharacter> ItemOwner;

	FDelegateHandle OnInteractInterruptedHandle;

	FDelegateHandle OnUseInterruptedHandle;

};
