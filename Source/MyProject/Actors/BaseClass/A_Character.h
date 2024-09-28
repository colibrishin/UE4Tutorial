#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"

#include "MyProject/Interfaces/PickingUp.h"
#include "GameFramework/Character.h"

#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Interfaces/AssetFetchable.h"

#include "A_Character.generated.h"

class AA_Collectable;
class USpringArmComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class UC_CharacterAsset;
class UC_Asset;
DECLARE_LOG_CATEGORY_EXTERN(LogCharacter , Log , All);

// Non-dynamic delegate due to forwarding to player state;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHandChanged, UChildActorComponent*);

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

	UC_CharacterAsset*      GetAssetComponent() const { return AssetComponent; }
	USkeletalMeshComponent* GetArmMesh() const { return ArmMeshComponent; }
	UChildActorComponent*   GetHand() const { return Hand; }
	bool                    IsHandBusy() const { return bHandBusy; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PickUp(UC_PickUp* InPickUp) override;

	virtual void Drop(UC_PickUp* InPickUp) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_Hand() const;

	UFUNCTION()
	void SetupHand(AActor* InChildActor) const;

protected:
	virtual void PostFetchAsset() override;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	bool bHandBusy;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UCameraComponent* Camera1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USkeletalMeshComponent* ArmMeshComponent;

	UPROPERTY(EditAnywhere)
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* JumpAction;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	UC_CharacterAsset* AssetComponent;

	UPROPERTY(VisibleAnywhere, Replicated)
	UChildActorComponent* Hand;

	UPROPERTY(VisibleAnywhere, Replicated)
	UChildActorComponent* ArmHand;

	FDelegateHandle CharacterForwardHandle;
	
};
