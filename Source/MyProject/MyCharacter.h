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

	class UMyInventoryComponent* GetInventory() const { return Inventory; }
	class UMyStatComponent*      GetStatComponent() const { return StatComponent; }

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

	bool TryPickWeapon(AMyWeapon* NewWeapon);

	void Attack(const float Value);

private:
	void ResetAttack();
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void HitscanAttack();
	void MeleeAttack();
	void Reload();

	void UpDown(const float Value);
	void LeftRight(const float Value);

	void Aim();
	void UnAim();

	void Interactive();
	void InteractInterrupted();

	void Use();
	void UseInterrupt();

	int32 GetDamage() const;
	void OnAttackAnimNotify();

	void Yaw(const float Value);
	void Pitch(const float Value);

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

	UPROPERTY(VisibleAnywhere)
	class AMyWeapon* Weapon;

	UPROPERTY(VisibleAnywhere)
	class UMyStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere)
	class UMyInventoryComponent* Inventory;

	UPROPERTY(VisibleAnywhere)
	class AMyCollectable* CurrentItem;

	FOnAttackEnded OnAttackEnded;

	FOnAiming OnAiming;

	FOnUseInterrupted OnUseInterrupted;

	FOnInteractInterrupted OnInteractInterrupted;
};
