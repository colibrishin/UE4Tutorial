// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyAIController.h"

#include "NavigationSystem.h"
#include "TimerManager.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "Engine/World.h"

void AMyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("AMyAIController::OnPossess"));

	GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle_MoveRandomly, 
		this, 
		&AMyAIController::MoveRandomly, 
		3.f, 
		true
	);
}

void AMyAIController::OnUnPossess()
{
	Super::OnUnPossess();

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_MoveRandomly);
}

void AMyAIController::MoveRandomly()
{
	const auto& TargetPawn = GetPawn();

	if (IsValid(TargetPawn))
	{
		const auto& NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());

		if (IsValid(NavSystem))
		{
			FNavLocation NextLocation;

			if (NavSystem->GetRandomPointInNavigableRadius(TargetPawn->GetActorLocation(), 500.f, NextLocation))
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TargetPawn is nullptr"));
	}
}
