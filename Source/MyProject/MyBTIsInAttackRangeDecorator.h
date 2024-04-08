// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "MyBTIsInAttackRangeDecorator.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBTIsInAttackRangeDecorator : public UBTDecorator
{
	GENERATED_BODY()

public:
	UMyBTIsInAttackRangeDecorator();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
