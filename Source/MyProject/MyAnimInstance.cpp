// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "MyCharacter.h"

#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"

UMyAnimInstance::UMyAnimInstance()
	: Speed(0),
	  Horizontal(0),
	  Vertical(0),
	  IsFalling(false)
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("AnimMontage'/Game/Blueprints/BPAnimationMontage.BPAnimationMontage'"));

	if (AM.Succeeded()) { AttackMontage = AM.Object; }
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const auto& Pawn = TryGetPawnOwner();

	if (!IsValid(Pawn))
	{
		return;
	}

	Speed = Pawn->GetVelocity().Size();

	const auto& Character = Cast<AMyCharacter>(Pawn);

	if (!IsValid(Character))
	{
		return;
	}

	IsFalling = Character->GetMovementComponent()->IsFalling();
	Vertical = Character->GetForwardInput();
	Horizontal = Character->GetRightInput();
}

FName UMyAnimInstance::GetAttackMontageSectionName(int32 NewIndex)
{
	return FName(*FString::Printf(TEXT("Attack%d"), NewIndex));
}

void UMyAnimInstance::SetAttackSection(int32 NewIndex)
{
	const auto& Section = GetAttackMontageSectionName(NewIndex);
	UE_LOG(LogTemp, Warning, TEXT("Section: %s"), *Section.ToString());
	Montage_JumpToSection(Section, AttackMontage);
}

void UMyAnimInstance::AnimNotify_Hit() const
{
	OnAttackHit.Broadcast();
}

void UMyAnimInstance::PlayAttackMontage(uint32 Index)
{
	Montage_Play(AttackMontage, 1.f);
	SetAttackSection(Index);
}
