// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"

#include "Actors/BaseClass/A_Character.h"

#include "GameFramework/PawnMovementComponent.h"

UMyAnimInstance::UMyAnimInstance()
	: Speed(0),
	  Horizontal(0),
	  Vertical(0),
	  Yaw(0),
	  Pitch(0),
	  IsFalling(false),
	  bIsAttacking(false),
	  bIsAiming(false),
	  bHasWeapon(false)
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM
		(TEXT("AnimMontage'/Game/Blueprints/BPAnimationMontage.BPAnimationMontage'"));

	if (AM.Succeeded()) { AttackMontage = AM.Object; }
}

void UMyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	const auto& Pawn = Cast<AA_Character>(TryGetPawnOwner());

	if (!IsValid(Pawn))
	{
		return;
	}

	//Pawn->BindOnAiming([this](const bool NewAiming)
	//{
	//	bIsAiming = NewAiming;
	//});
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

	const auto& Character = Cast<AA_Character>(Pawn);

	if (!IsValid(Character))
	{
		return;
	}

	IsFalling = Character->GetMovementComponent()->IsFalling();

	const auto& Velocity = Character->GetVelocity();

	Vertical = FVector::DotProduct(Velocity, Character->GetActorForwardVector());
	Horizontal = FVector::DotProduct(Velocity, Character->GetActorRightVector());
	Yaw = Character->GetActorRotation().Yaw;
	//Pitch = Character->GetPitchInput();
	bHasWeapon = Character->IsHandBusy();
}

FName UMyAnimInstance::GetAttackMontageSectionName(const int32 NewIndex)
{
	return FName(*FString::Printf(TEXT("Attack%d"), NewIndex));
}

void UMyAnimInstance::SetAttackSection(const int32 NewIndex)
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