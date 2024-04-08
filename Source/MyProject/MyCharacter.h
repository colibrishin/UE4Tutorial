// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEnded)

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
	static const FName HeadSocketName;

	// Sets default values for this character's properties
	AMyCharacter();

	DECL_BINDON(OnAttackEnded)
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

	void Attack();

private:
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void UpDown(const float Value);
	void LeftRight(const float Value);

	void Interactive();

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

	FOnAttackEnded OnAttackEnded;
};
