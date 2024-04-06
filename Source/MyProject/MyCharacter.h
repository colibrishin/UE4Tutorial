// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class AMyWeapon;

UCLASS()
class MYPROJECT_API AMyCharacter : public ACharacter
{
	// Pawn을 상속받는 확장된 클래스
	// 스켈레탈 메쉬, 애니메이션 등까지 포함됨

	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float GetForwardInput() const { return ForwardInput; }
	float GetRightInput() const { return RightInput; }

	void TryPickWeapon(AMyWeapon* NewWeapon);

private:
	static const FName WeaponSocketName;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void UpDown(const float Value);
	void LeftRight(const float Value);

	void Attack();

	void OnHit() const;

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

};
