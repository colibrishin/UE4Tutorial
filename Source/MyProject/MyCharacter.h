// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"
#include "Enum.h"

#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEnded)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAiming, bool)
DECLARE_MULTICAST_DELEGATE(FOnUseInterrupted)
DECLARE_MULTICAST_DELEGATE(FOnInteractInterrupted)

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

	DECL_BINDON(OnAttackEnded)
	DECL_BINDON(OnAiming, bool)
	DECL_BINDON(OnUseInterrupted)
	DECL_BINDON(OnInteractInterrupted)

	class UMyInventoryComponent* GetInventory() const;
	class UMyStatComponent* GetStatComponent() const;
	class AMyWeapon* GetWeapon() const;
	class AMyCollectable* GetCurrentItem() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float GetForwardInput() const { return ForwardInput; }
	float GetRightInput() const { return RightInput; }

	bool TryPickWeapon(AMyWeapon* NewWeapon) const;

	void Attack(const float Value);

private:

	// ============ Attacking ============
	UFUNCTION(Server, Reliable)
	void Server_Attack(const float Value);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Attack(const float Value);

	void AttackStart(const float Value);

	void HitscanAttack();
	void MeleeAttack();
	void ResetAttack();

	int32 GetDamage() const;
	void OnAttackAnimNotify();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// ============ End of Attacking ============


	// ============ Reloading ============
	void Reload();

	UFUNCTION(Server, Reliable)
	void Server_Reload();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Reload();

	void ReloadStart() const;

	// ============ End of Reloading ============

	void UpDown(const float Value);
	void LeftRight(const float Value);

	void Aim();
	void UnAim();

	// ============ Interacting ============

	void Interactive();

	UFUNCTION(Server, Reliable)
	void Server_Interactive();

	void InteractiveImpl();

	void InteractInterrupted();

	UFUNCTION(Server, Reliable)
	void Server_InteractInterrupted();

	void InteractInterruptedImpl();

	// ============ End of Interacting ============

	// ============ Using ============

	void Use();

	UFUNCTION(Server, Reliable)
	void Server_Use();

	void UseImpl();

	void UseInterrupt();

	UFUNCTION(Server, Reliable)
	void Server_UseInterrupt();

	void UseInterruptImpl();

	// ============ End of Using ============

	void Yaw(const float Value);
	void Pitch(const float Value);
	bool IsBuyMenuOpened() const;

	UPROPERTY(VisibleAnywhere)
	float ForwardInput;

	UPROPERTY(VisibleAnywhere)
	float RightInput;

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

	FOnAttackEnded OnAttackEnded;

	FOnAiming OnAiming;

	FOnUseInterrupted OnUseInterrupted;

	FOnInteractInterrupted OnInteractInterrupted;

	FDelegateHandle OnAttackEndedHandle;
};
