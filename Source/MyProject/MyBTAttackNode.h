// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MyBTAttackNode.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBTAttackNode : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMyBTAttackNode();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds) override;

private:
	bool bIsAttacking;

};
