#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"

#include "MyProject/Interfaces/PickingUp.h"
#include "GameFramework/Character.h"
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
class UC_Health;
DECLARE_LOG_CATEGORY_EXTERN(LogCharacter , Log , All);

// Non-dynamic delegate due to forwarding to player state;
DECLARE_MULTICAST_DELEGATE_OneParam( FOnHandChanged , AA_Collectable* );

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
	UC_Health* GetHealthComponent() const { return HealthComponent; }
	USkeletalMeshComponent* GetArmMesh() const { return ArmMeshComponent; }
	UChildActorComponent*   GetHand() const { return Hand; }
	bool                    IsHandBusy() const { return bHandBusy; };

	virtual float TakeDamage( float DamageAmount , FDamageEvent const& DamageEvent , AController* EventInstigator , AActor* DamageCauser ) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PickUp(UC_PickUp* InPickUp, const IPickingUp::PickUpSpawnedPredicate& InObjectPredicate, const PickUpSpawnedPredicate& InPostSpawnPredicate ) override;

	virtual void Drop(UC_PickUp* InPickUp) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	bool TestCollectable( TArray<FOverlapResult>& OutResults );

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
	void SyncHandProperties() const;

	UFUNCTION()
	void UpdateOnHandChangedTime( AA_Collectable* InHandActor );

protected:
	virtual void PostFetchAsset() override;

	UFUNCTION(Server, Reliable)
	void Server_PickUp();

	UFUNCTION(Server, Reliable)
	void Server_Drop();

	UFUNCTION( Server , Reliable )
	void Server_Interactive();

	UFUNCTION( Server , Reliable )
	void Server_StopInteractive();
	
	UPROPERTY(VisibleAnywhere, Replicated)
	bool bHandBusy;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UCameraComponent* Camera1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USkeletalMeshComponent* ArmMeshComponent;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	UC_CharacterAsset* AssetComponent;

	UPROPERTY( VisibleAnywhere , Replicated , BlueprintReadWrite )
	UC_Health* HealthComponent;

	UPROPERTY(VisibleAnywhere, Replicated)
	UChildActorComponent* Hand;

	UPROPERTY(VisibleAnywhere, Replicated)
	UChildActorComponent* ArmHand;
	
	// Properties that allows not to iterate the child actors;
#pragma region ChildActors
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Hand)
	AA_Collectable* HandActor;

	// Since ArmHand is duplication of hand actor, the availability of ArmHandActor can be assumed by OnHandChanged;
	UPROPERTY(VisibleAnywhere, Replicated)
	AA_Collectable* ArmHandActor;
#pragma endregion

	// Server side only variable that produce and consume the data between the pick up and interactive.
	// It will be used to prevent the pick up and interactive in the same tick.
	UPROPERTY( VisibleAnywhere )
	float LastHandChangedInServerTime;

	// Server side only variable that stores the latest successful pick up or drop.
	// Note that this is not the hand actor but the template actor that has used for the cloning.
	UPROPERTY( VisibleAnywhere )
	AA_Collectable* LastSuccededPickOrDrop;
	
	UPROPERTY( EditAnywhere )
	UInputMappingContext* MovementInputMapping;

	UPROPERTY( EditAnywhere )
	UInputMappingContext* UtilityInputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* JumpAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInputAction* PickUpAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInputAction* DropAction;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite )
	UInputAction* InteractiveAction;

	FDelegateHandle CharacterForwardHandle;
	
};
