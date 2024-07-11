// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"
#include "Enum.h"

#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAiming, bool, bAim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseInterrupted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractInterrupted);

class UMyStatComponent;
class AMyWeapon;

UCLASS()
class MYPROJECT_API AMyCharacter : public ACharacter
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

	class AMyWeapon* TryGetWeapon() const;
	class AMyItem* TryGetItem() const;
	class AMyCollectable* GetCurrentHand() const;
	class USkeletalMeshComponent* GetArmMeshComponent() const;
	class UMyInventoryComponent* GetInventory() const;
	class UMyStatComponent* GetStatComponent() const;
	
	FOnAttackStarted OnAttackStarted;

	FOnAttackEnded OnAttackEnded;

	FOnAiming OnAiming;

	FOnUseInterrupted OnUseInterrupted;

	FOnInteractInterrupted OnInteractInterrupted;

	float GetPitchInput() const { return PitchInput; }

	void OnHandChanged(class AMyCollectable* Previous, class AMyCollectable* New, class AMyPlayerState* ThisPlayerState);

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Attack(const float Value);

	void ResetAttack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

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

	int32 GetDamage() const;
	void OnAttackAnimNotify();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(Server, Reliable)
	void Server_AttackInterrupted(const float Value);

	// ============ End of Attacking ============


	// ============ Reloading ============

	void Reload();

	UFUNCTION(Server, Reliable)
	void Server_Reload();

	UFUNCTION(Client, Reliable)
	void Client_Reload();

	// ============ End of Reloading ============

	void UpDown(const float Value);
	void LeftRight(const float Value);

	void Aim();
	void UnAim();

	// ============ Interacting ============

	void Interactive();

	UFUNCTION(Server, Reliable)
	void Server_Interactive();

	void InteractInterrupted();

	UFUNCTION(Server, Reliable)
	void Server_InteractInterrupted();

	UFUNCTION(Client, Reliable)
	void Client_InteractInterrupted();

	// ============ End of Interacting ============

	// ============ Using ============

	void Use();

	void UseInterrupt();

	UFUNCTION(Server, Reliable)
	void Server_UseInterrupt();

	UFUNCTION(Client, Reliable)
	void Client_UseInterrupted();

	// ============ End of Using ============

	void AttachArmCollectable(class AMyCollectable* Previous, class AMyCollectable* New);

	// ============ Swap ============

	void SwapPrimary();
	UFUNCTION(Server, Reliable)
	void Server_SwapPrimary();

	void SwapSecondary();
	UFUNCTION(Server, Reliable)
	void Server_SwapSecondary();

	void SwapMelee();
	UFUNCTION(Server, Reliable)
	void Server_SwapMelee();

	void SwapUtility();
	UFUNCTION(Server, Reliable)
	void Server_SwapUtility();

	void SwapBomb();
	UFUNCTION(Server, Reliable)
	void Server_SwapBomb();

	void WeaponSwap(const int32 Index) const;

	// ============ End of Swap ============

	void Yaw(const float Value);
	void Pitch(const float Value);
	bool IsBuyMenuOpened() const;

	UFUNCTION(Server, Unreliable)
	void Server_SyncPitch(const float NewPitch);

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

	UPROPERTY(VisibleAnywhere)
	int32 AttackIndex;

	// Pawn에서 직접 추가했던 무브먼트 컴포넌트는 필요없음
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	class UMyAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* ArmMeshComponent;

	UPROPERTY(VisibleAnywhere)
	class UMyAnimInstance* ArmAnimInstance;

	UPROPERTY(VisibleAnywhere, Replicated)
	class AMyCollectable* HandCollectable;

	UPROPERTY(VisibleAnywhere)
	class USoundWave* FootstepSound;
	
};
