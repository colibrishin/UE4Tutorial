// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MyBTPatrolVectorNode.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBTPatrolVectorNode : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMyBTPatrolVectorNode();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
