#pragma once

#include "CoreMinimal.h"
#include "MyProject/Interfaces/PickingUp.h"
#include "GameFramework/Character.h"

#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Interfaces/AssetFetchable.h"

#include "A_Character.generated.h"

DECLARE_LOG_CATEGORY_EXTERN( LogCharacter , Log, All );

// Non-dynamic delegate due to forwarding to player state;
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHandChanged, UC_PickUp*, UC_PickUp*);

class UInputMappingContext;

UCLASS()
class MYPROJECT_API AA_Character : public ACharacter, public IPickingUp, public IAssetFetchable
{
	GENERATED_BODY()

public:
	static const FName LeftHandSocketName;
	static const FName RightHandSocketName;
	static const FName HeadSocketName;
	static const FName ChestSocketName;

	// Sets default values for this character's properties
	AA_Character();

	FOnHandChanged OnHandChanged;
	
	USkeletalMeshComponent* GetArmMesh() const { return ArmMeshComponent; }
	UC_PickUp*              GetHand() const { return Hand; }
	bool                    IsHandBusy() const { return bHandBusy; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PickUp(UC_PickUp* InPickUp) override;

	virtual void Drop(UC_PickUp* InPickUp) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_Hand(UC_PickUp* InOldHand) const;
	
protected:
	UPROPERTY(VisibleAnywhere, Replicated)
	bool bHandBusy;
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	USkeletalMeshComponent* ArmMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UInputMappingContext* InputMapping;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Hand)
	UC_PickUp* Hand;
	
};
