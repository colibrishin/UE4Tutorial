// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyBTAttackNode.h"

#include "AIController.h"
#include "MyCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"

UMyBTAttackNode::UMyBTAttackNode()
	: bIsAttacking(false)
{
	bNotifyTick = true;

}

EBTNodeResult::Type UMyBTAttackNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto& SuperResult = Super::ExecuteTask(OwnerComp , NodeMemory);
	const auto& AIOwner = Cast<AMyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	constexpr wchar_t TargetKey[] = L"Target";

	if (!IsValid(AIOwner))
	{
		return EBTNodeResult::Failed;
	}

	const auto& Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey));

	if (!IsValid(Target))
	{
		return EBTNodeResult::Failed;
	}

	AIOwner->Attack(1.f);
	bIsAttacking = true;

	return SuperResult;
}

void UMyBTAttackNode::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp , NodeMemory , DeltaSeconds);

	if (!bIsAttacking)
	{
		FinishLatentTask(OwnerComp , EBTNodeResult::Succeeded);
	}
}

void UMyBTAttackNode::OnInstanceCreated(UBehaviorTreeComponent& OwnerComp)
{
	Super::OnInstanceCreated(OwnerComp);

	const auto& AIOwner = Cast<AMyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (!IsValid(AIOwner)) { return; }

	AIOwner->OnAttackEnded.AddUniqueDynamic(this, &UMyBTAttackNode::OnAttackEnded);
}

void UMyBTAttackNode::OnAttackEnded()
{
	bIsAttacking = false;
}
