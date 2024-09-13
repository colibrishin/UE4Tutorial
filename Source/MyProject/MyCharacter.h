// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PickableObject.h"
#include "MyCharacter.generated.h"

class UMyAnimInstance;
class UMyInventoryComponent;
class AMyCollectable;
class AMyItem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAiming, bool, bAim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseInterrupted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractInterrupted);

class UMyStatComponent;
class AMyWeapon;

UCLASS()
class MYPROJECT_API AMyCharacter : public ACharacter, public IPickableObject
{
	// Pawn을 상속받는 확장된 클래스
	// 스켈레탈 메쉬, 애니메이션 등까지 포함됨

	GENERATED_BODY()

public:
	static const FName LeftHandSocketName;
	static const FName RightHandSocketName;
	static const FName HeadSocketName;
	static const FName ChestSocketName;

	// Sets default values for this character's properties
	AMyCharacter();

	USkeletalMeshComponent* GetArmMeshComponent() const;
	UMyInventoryComponent* GetInventory() const;
	UMyStatComponent* GetStatComponent() const;
	
	FOnAttackStarted OnAttackStarted;

	FOnAttackEnded OnAttackEnded;

	FOnAiming OnAiming;

	FOnUseInterrupted OnUseInterrupted;

	FOnInteractInterrupted OnInteractInterrupted;

	float GetPitchInput() const { return PitchInput; }

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Attack(const float Value);

	UFUNCTION()
	void ResetAttack();

	void UpdateMeshAnimInstance();
	
	void UpdateArmMeshAnimInstance();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Landed(const FHitResult& Hit) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float GetForwardInput() const { return ForwardInput; }
	float GetRightInput() const { return RightInput; }

private:

	friend void CharacterSwapHand(AMyCharacter* Character, const int Index);

	// ============ Attacking ============

	UFUNCTION(Server, Reliable)
	void Server_Attack(const float Value);
	UFUNCTION(Client, Reliable)
	void Client_Attack();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_MeleeAttack();

	UFUNCTION()
	void OnAttackAnimNotify();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// ============ End of Attacking ============

	void UpDown(const float Value);
	void LeftRight(const float Value);

	void Yaw(const float Value);
	void Pitch(const float Value);
	bool IsBuyMenuOpened() const;

	UFUNCTION(Server, Unreliable)
	void Server_SyncPitch(const float NewPitch);

public:
	virtual void PickUp(UC_PickUp* InPickUp) override;
	
	virtual void Drop(UC_PickUp* InPickUp) override;

private:
	UPROPERTY(VisibleAnywhere)
	float PreviousAttack;

	UPROPERTY(VisibleAnywhere)
	float ForwardInput;

	UPROPERTY(VisibleAnywhere)
	float RightInput;

	// todo: overhead?
	UPROPERTY(VisibleAnywhere, Replicated)
	float PitchInput;
	
	UPROPERTY(VisibleAnywhere)
	bool IsAttacking;

	UPROPERTY(VisibleAnywhere)
	bool IsAiming;

	UPROPERTY(VisibleAnywhere)
	bool CanAttack;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bHandBusy;

	UPROPERTY(VisibleAnywhere)
	int32 AttackIndex;

	// Pawn에서 직접 추가했던 무브먼트 컴포넌트는 필요없음
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	UMyAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* ArmMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UMyAnimInstance* ArmAnimInstance;

	UPROPERTY(VisibleAnywhere)
	USoundWave* FootstepSound;
	
};
