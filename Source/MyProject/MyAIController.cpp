// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyAIController.h"

#include "NavigationSystem.h"
#include "TimerManager.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "Engine/World.h"

#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"

AMyAIController::AMyAIController()
{
	// No need to add _C suffix
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT_MyCharacter(TEXT("BehaviorTree'/Game/Blueprints/AI/BTMyCharacter.BTMyCharacter'"));

	if (BT_MyCharacter.Succeeded())
	{
		BehaviorTree = BT_MyCharacter.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BB_MyCharacter(TEXT("BlackboardData'/Game/Blueprints/AI/BBMyCharacter.BBMyCharacter'"));

	if (BB_MyCharacter.Succeeded())
	{
		BlackboardData = BB_MyCharacter.Object;
	}
}

void AMyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("AMyAIController::OnPossess"));

	/*GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle_MoveRandomly, 
		this, 
		&AMyAIController::MoveRandomly, 
		3.f, 
		true
	);*/

	if (UseBlackboard(BlackboardData, Blackboard))
	{
		if (!RunBehaviorTree(BehaviorTree))
		{
			UE_LOG(LogTemp, Error, TEXT("RunBehaviorTree is failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UseBlackboard is failed"));
	}
}

void AMyAIController::OnUnPossess()
{
	Super::OnUnPossess();

	//GetWorld()->GetTimerManager().ClearTimer(TimerHandle_MoveRandomly);
}
