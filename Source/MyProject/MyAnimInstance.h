// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackHit)

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UMyAnimInstance();

	void PlayAttackMontage(uint32 Index);

	template <typename T, typename ObjectLock = std::enable_if_t<std::is_base_of_v<UObject, T>>>
	void ListenForAttackHit(const T* Obj, void (T::*Func)() const)
	{
		OnAttackHit.AddUObject(Obj, Func);
	}

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	static FName GetAttackMontageSectionName(int32 NewIndex);
	void SetAttackSection(int32 NewIndex);

	UFUNCTION()
	void AnimNotify_Hit() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, Meta=(AllowPrivateAccess))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, Meta=(AllowPrivateAccess))
	float Horizontal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess))
	float Vertical;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, Meta=(AllowPrivateAccess))
	bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, Meta=(AllowPrivateAccess))
	UAnimMontage* AttackMontage;

	FOnAttackHit OnAttackHit;
};
