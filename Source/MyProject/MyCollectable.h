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

	UStaticMeshComponent* GetMesh() const { return Mesh; }
	class UBoxComponent* GetCollider() const { return Collider; }

	virtual bool Interact(class AMyCharacter* Character) override final;
	virtual bool Drop();

	void Hide() const;
	void Show() const;
	void ShowOnly() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual bool InteractImpl(class AMyCharacter* Character) PURE_VIRTUAL(AMyCollectable::InteractImpl, return false;);

	virtual bool OnCharacterOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, class AMyCharacter* Character, const FHitResult& SweepResult
	);

	bool IsBelongToCharacter() const;

	void SetItemOwner(class AMyCharacter* FutureOwner);

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

	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<class AMyCharacter> ItemOwner;

};
