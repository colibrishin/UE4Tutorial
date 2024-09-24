// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/AI/MyBTSearchTargetService.h"

#include "AIController.h"
#include "MyProject/Actors/BaseClass/A_Character.h"

#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Engine/OverlapResult.h"

UMyBTSearchTargetService::UMyBTSearchTargetService()
{
	NodeName = TEXT("Search Target");
	Interval = 1.0f;
}

void UMyBTSearchTargetService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp , NodeMemory , DeltaSeconds);
	constexpr wchar_t Key[] = L"Target";

	const auto& CurrentTarget = OwnerComp.GetAIOwner()->GetPawn();

	if (IsValid(CurrentTarget))
	{
		const auto& World = CurrentTarget->GetWorld();
		const auto& Position = CurrentTarget->GetActorLocation();
		constexpr float Radius = 500.0f;

		TArray<FOverlapResult> Overlaps;

		const FCollisionQueryParams Params(NAME_None , false , CurrentTarget);

		const auto& Result = World->OverlapMultiByChannel
		(
			OUT Overlaps,
			Position,
			FQuat::Identity,
			ECC_EngineTraceChannel2,
			FCollisionShape::MakeSphere(Radius),
			Params
		);

		if (Result)
		{
			for (const auto& Overlap : Overlaps)
			{
				const auto& Character = Cast<AA_Character>(Overlap.GetActor());

				if (IsValid(Character))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(Key, Character);
					DrawDebugSphere(World, Position, Radius, 32, FColor::Green, false, 0.5f);
					return;
				}
			}
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(Key, nullptr);
		}

		DrawDebugSphere(World, Position, Radius, 32, FColor::Red, false, 0.5f);
	}
}
