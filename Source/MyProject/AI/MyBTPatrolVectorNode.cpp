// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyBTPatrolVectorNode.h"

#include <string>

#include "AIController.h"
#include "NavigationSystem.h"

#include "BehaviorTree/BlackboardComponent.h"

UMyBTPatrolVectorNode::UMyBTPatrolVectorNode()
{
	NodeName = TEXT("PatrolVectorNode");
}

EBTNodeResult::Type UMyBTPatrolVectorNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto& SuperResult = Super::ExecuteTask(OwnerComp, NodeMemory);

	const auto& TargetPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (IsValid(TargetPawn))
	{
		const auto& NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());

		if (IsValid(NavSystem))
		{
			FNavLocation NextLocation;

			if (NavSystem->GetRandomPointInNavigableRadius(TargetPawn->GetActorLocation(), 500.f, NextLocation))
			{
				constexpr wchar_t KeyName[] = L"PatrolVector";
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(KeyName, NextLocation.Location);
				return EBTNodeResult::Succeeded;
			}
		}

		return EBTNodeResult::Failed;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TargetPawn is nullptr"));
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
