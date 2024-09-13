// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "MyCollectable.generated.h"

class UMyCollectableDataAsset;
class UMyCollectableComponent;

UCLASS()
class MYPROJECT_API AMyCollectable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCollectable();

	USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }
	UMyCollectableComponent* GetCollectableComponent() const { return CollectableComponent; }

	class UBoxComponent* GetCollider() const { return Collider; }
	class AMyCharacter* GetItemOwner() const;

	UFUNCTION(Reliable, Server)
	void Server_Drop();

	UFUNCTION(Reliable, NetMulticast)
	void Multi_Drop();

	UFUNCTION(Reliable, Server)
	void Server_Interact(AMyCharacter* Character);

	UFUNCTION(Reliable, Server)
	void Server_Use(AMyCharacter* Character);

	UFUNCTION(Reliable, Server)
	void Server_InteractInterrupted();

	UFUNCTION(Reliable, Server)
	void Server_UseInterrupted();

	void Hide() const;
	void Show() const;

	virtual void UpdateAsset();

	bool IsBelongToCharacter() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual bool OnCharacterOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, class AMyCharacter* Character, const FHitResult& SweepResult
	);

	virtual void Multi_Drop_Implementation();

	virtual void Server_Drop_Implementation();

	virtual void Client_TryAttachItem_Implementation(AMyCharacter* Character);

	virtual void Multi_Interact_Implementation(AMyCharacter* Character);

	virtual void Server_Interact_Implementation(class AMyCharacter* Character);

	virtual void Server_Use_Implementation(class AMyCharacter* Character);
	
	virtual void Server_InteractInterrupted_Implementation();

	virtual void Server_UseInterrupted_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Interact(AMyCharacter* Character);

	virtual bool PreInteract(class AMyCharacter* Character);
	virtual bool TryAttachItem(AMyCharacter* Character);
	virtual bool PostInteract(class AMyCharacter* Character);

	UFUNCTION(Reliable, Client)
	void Client_TryAttachItem(AMyCharacter* Character);

	UFUNCTION(Reliable, Client)
	void Client_UnbindInterruption();

	virtual bool PreUse(AMyCharacter* Character);
	virtual bool PostUse(AMyCharacter* Character);

	virtual void DropBeforeCharacter();
	virtual void DropLocation();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	UMyCollectableComponent* CollectableComponent;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	virtual void OnCharacterOverlapImpl(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Collider;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMeshComponent;

};
