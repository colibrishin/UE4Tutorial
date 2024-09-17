// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/AI/MyBTIsInAttackRangeDecorator.h"

#include "AIController.h"
#include "MyProject/Actors/BaseClass/A_Character.h"

#include "BehaviorTree/BlackboardComponent.h"

UMyBTIsInAttackRangeDecorator::UMyBTIsInAttackRangeDecorator()
{
	NodeName = TEXT("IsInAttackRange");
}

bool UMyBTIsInAttackRangeDecorator::CalculateRawConditionValue(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory
) const
{
	const auto& RawResult = Super::CalculateRawConditionValue(OwnerComp , NodeMemory);
	const auto& AIOwner = OwnerComp.GetAIOwner()->GetPawn();

	constexpr wchar_t Key[] = TEXT("Target");
	constexpr float Range = 100.f;

	if (!IsValid(AIOwner))
	{
		return false;
	}

	const auto& Target = Cast<AA_Character>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(Key));

	if (!IsValid(Target))
	{
		return false;
	}

	if (Target->GetDistanceTo(AIOwner) <= 150.f)
	{
		return true;
	}

	return false;
}
