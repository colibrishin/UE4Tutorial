// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyC4.h"

#include "ConstantFVector.hpp"
#include "DrawDebugHelpers.h"
#include "MyBombIndicatorWidget.h"
#include "MyBombProgressWidget.h"
#include "MyCharacter.h"
#include "MyInGameHUD.h"
#include "MyInGameWidget.h"
#include "MyInventoryComponent.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PawnMovementComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AMyC4::AMyC4()
	: BombState(EMyBombState::Idle),
      Elapsed(0.f),
      PlantingTime(0.f),
      DefusingTime(0.f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_C4(TEXT("StaticMesh'/Game/Models/C4-1.C4-1'"));

	if (SM_C4.Succeeded())
	{
		Cast<UStaticMeshComponent>(GetMesh())->SetStaticMesh(SM_C4.Object);
	}
}

bool AMyC4::IsPlantable(const bool bCheckSpeed) const
{
	FHitResult            HitResult;
	FCollisionQueryParams Params{NAME_None , false , this};

	UE_LOG(LogTemp, Warning, TEXT("%s"), *GetActorLocation().ToString());

	const auto& GroundResult = GetWorld()->LineTraceSingleByProfile
	(
		OUT HitResult,
		GetActorLocation(),
		GetActorLocation() + (FConstantFVector::DownVector * 1000.f),
		TEXT("IgnoreOnlyPawn"),
		Params
	);

	const auto& OverlapResult = GetWorld()->OverlapAnyTestByChannel
	(
		GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel6,
		FCollisionShape::MakeSphere(100.f),
		Params
	);

	float Speed = 0.f;

	if (bCheckSpeed)
	{
		Speed = 1.f;

		if (IsValid(GetItemOwner()))
		{
			Speed = GetItemOwner()->GetVelocity().Size();
		}
	}

	DrawDebugLine
	(
		GetWorld(),
		GetActorLocation(),
		HitResult.Location,
		GroundResult ? FColor::Green : FColor::Red,
		false,
		1.f
	);

	if (IsPlanted())
	{
		LOG_FUNC(LogTemp, Warning, "Bomb is already planted");
	}

	if (IsExploded())
	{
		LOG_FUNC(LogTemp, Warning, "Bomb is exploded");
	}

	if (!GroundResult)
	{
		LOG_FUNC(LogTemp, Warning, "Ground is not found");
	}

	if (!OverlapResult)
	{
		LOG_FUNC(LogTemp, Warning, "Bomesite is not found");
	}

	if (Speed != 0.f)
	{
		LOG_FUNC(LogTemp, Warning, "Speed is not zero");
	}


	return !IsPlanted() && !IsExploded() && GroundResult && OverlapResult && (Speed == 0.f);
}

bool AMyC4::IsDefusable(const bool bCheckSpeed) const
{
	const FCollisionQueryParams Params{NAME_None, false, this};
	const FCollisionObjectQueryParams ObjectParams(ECollisionChannel::ECC_Pawn);

	TArray<FOverlapResult> OverlapResult;

	const auto& DefuserCheck = GetWorld()->OverlapMultiByObjectType
	(
		OverlapResult,
		GetActorLocation(),
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(50.f),
		Params
	);

	bool IsDefuserNearby = false;

	for (const auto& Result : OverlapResult)
	{
		const auto& Actor = Result.GetActor();

		UE_LOG(LogTemp, Warning, TEXT("NearBy Actor: %s"), *Actor->GetName());

		if (Actor == DefusingCharacter)
		{
			IsDefuserNearby = true;
			break;
		}
	}

	float Speed = 0.f;

	if (bCheckSpeed)
	{
		Speed = 1.f;

		if (DefusingCharacter.IsValid())
		{
			Speed = DefusingCharacter->GetVelocity().Size();
		}
	}

	DrawDebugSphere
		(
		 GetWorld(),
		 GetActorLocation(),
		 50.f,
		 16,
		 IsDefuserNearby ? FColor::Green : FColor::Red,
		 false,
		 1.f
		);

	if (!IsDefuserNearby)
	{
		LOG_FUNC(LogTemp, Warning, "Defuser is not nearby");
	}

	if (Speed != 0.f)
	{
		LOG_FUNC(LogTemp, Warning, "Speed is not zero");
	}


	return IsDefuserNearby && (Speed == 0.f);
}

// Called when the game starts or when spawned
void AMyC4::BeginPlay()
{
	Super::BeginPlay();
}

void AMyC4::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyC4, Elapsed);
	DOREPLIFETIME(AMyC4, PlantingTime);
	DOREPLIFETIME(AMyC4, DefusingTime);
	DOREPLIFETIME(AMyC4, BombState);
	DOREPLIFETIME(AMyC4, DefusingCharacter);
}

void AMyC4::ClientInteractImpl(AMyCharacter* Character)
{
	Super::ClientInteractImpl(Character);
	
	TryDefuse(Character);
}

void AMyC4::ClientUseImpl(AMyCharacter* Character)
{
	Super::ClientUseImpl(Character);

	TryPlant(Character);
}

void AMyC4::ClientInteractInterruptedImpl()
{
	Super::ClientInteractInterruptedImpl();
}

void AMyC4::ClientUseInterruptedImpl()
{
	Super::ClientUseInterruptedImpl();
}

void AMyC4::OnBombExplodedImpl()
{
	if (HasAuthority())
	{
		if (IsDefusing())
		{
			DefusingCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			DefusingCharacter->UnbindOnAttackStarted(DefuserAttackHandle);
			DefusingCharacter->UnbindOnInteractInterrupted(DefuserOnInteractInterruptedHandle);

			SetDefusing(nullptr);
		}

		SetState(EMyBombState::Exploded);
		UE_LOG(LogTemp, Warning, TEXT("Bomb IsExploded"));
		GetWorldTimerManager().ClearTimer(OnBombExplodedHandle);
	}
}

void AMyC4::OnBombPlantedImpl()
{
	if (HasAuthority())
	{
		if (!IsValid(GetItemOwner()))
		{
			LOG_FUNC(LogTemp, Warning, "Item owner is not valid");
			PlantingTime = 0.f;
			return;
		}

		const auto& GroundResult = IsPlantable();

		if (!GroundResult)
		{
			LOG_FUNC(LogTemp, Warning, "Bomb is not plantable");
			PlantingTime = 0.f;
			return;
		}

		LOG_FUNC(LogTemp, Warning, "Bomb planted");
		SetState(EMyBombState::Planted);
		Elapsed = 0.f;

		GetMesh()->SetSimulatePhysics(false);
		GetItemOwner()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		Drop();

		GetWorldTimerManager().ClearTimer(OnBombPlantingHandle);

		GetWorldTimerManager().SetTimer(
			OnBombExplodedHandle,
			this,
			&AMyC4::OnBombExplodedImpl,
			FullExplodingTime,
			false);
	}
}

void AMyC4::OnBombDefusedImpl()
{
	if (HasAuthority())
	{
		if (!DefusingCharacter.IsValid())
		{
			LOG_FUNC(LogTemp, Warning, "Defusing character is not valid");
			DefusingTime = 0.f;
			return;
		}

		// Second chance, preventing from not InteractInterrupted
		if (!IsDefusing() || !IsDefusable())
		{
			LOG_FUNC(LogTemp, Warning, "Bomb is not defusable");
			DefusingTime = 0.f;
			return;
		}

		DefusingCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		SetState(EMyBombState::Defused);
		LOG_FUNC(LogTemp, Warning, "Bomb defused");
		GetWorldTimerManager().ClearTimer(OnBombExplodedHandle);
		GetWorldTimerManager().ClearTimer(OnBombDefusingHandle);
	}
}

void AMyC4::Destroyed()
{
	Super::Destroyed();

	GetWorldTimerManager().ClearTimer(OnBombPlantingHandle);
	GetWorldTimerManager().ClearTimer(OnBombDefusingHandle);
	GetWorldTimerManager().ClearTimer(OnBombExplodedHandle);
}

bool AMyC4::PreInteract(AMyCharacter* Character)
{
	if (!IsPlanted())
	{
		if (IsDefused() || IsExploded())
		{
			UE_LOG(LogTemp, Warning, TEXT("Bomb is exploded or defused"));
			return false;
		}
	}

	if (IsPlanted())
	{
		return TDefuseGuard(Character) && Super::PreInteract(Character);
	}

	return Super::PreInteract(Character);
}

bool AMyC4::TDefuseGuard(AMyCharacter* Character) const
{
	if (const auto& PlayerState = Character->GetPlayerState<AMyPlayerState>())
	{
		if (PlayerState->GetTeam() == EMyTeam::T)
		{
			LOG_FUNC(LogTemp, Warning, "T can't defuse a bomb or pick up a bomb after planted");
			return false;
		}

		return true;
	}

	return false;
}

bool AMyC4::CTPickPlantGuard(AMyCharacter* Character) const
{
	if (const auto& PlayerState = Character->GetPlayerState<AMyPlayerState>())
	{
		if (PlayerState->GetTeam() == EMyTeam::CT)
		{
			LOG_FUNC(LogTemp, Warning, "CT can't pick a bomb or plant");
			return false;
		}

		return true;
	}

	return false;
}

bool AMyC4::PostInteract(AMyCharacter* Character)
{
	if (IsPlanted())
	{
		return TDefuseGuard(Character) && TryDefuse(Character);
	}
	else
	{
		const auto& Result = CTPickPlantGuard(Character) && Super::PostInteract(Character);

		if (Result)
		{
			if (HasAuthority())
			{
				Multi_NotifyPicked(Character);
			}
		}

		return Result;
	}
}

bool AMyC4::TryAttachItem(const AMyCharacter* Character)
{
	return Super::TryAttachItem(Character);
}

bool AMyC4::PreUse(AMyCharacter* Character)
{
	const auto& Result = Super::PreUse(Character);

	if (!IsPlanted())
	{
		if (IsExploded() || IsDefused())
		{
			UE_LOG(LogTemp, Warning, TEXT("Bomb is exploded or defused"));
			return false && Result;
		}

		return Result && CTPickPlantGuard(Character);
	}

	return Result;
}

bool AMyC4::TryPlant(class AMyCharacter* Character)
{
	if (IsPlanted())
	{
		return false;
	}

	if (!IsPlantable(false))
	{
		return false;
	}

	Character->GetMovementComponent()->StopMovementImmediately();
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	PlanterAttackHandle = GetItemOwner()->BindOnAttackStarted(this, &AMyC4::UseInterrupted);
	
	UE_LOG(LogTemp, Warning, TEXT("Bomb planting"));

	if (OnBombPlantingHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(OnBombPlantingHandle);
	}

	GetWorldTimerManager().SetTimer
	(
        OnBombPlantingHandle, 
        this,
        &AMyC4::OnBombPlantedImpl,
        FullPlantingTime,
        false
	);

	SetPlanting(true);

	return true;
}

void AMyC4::Multi_NotifyPicked_Implementation(AMyCharacter* Character) const
{
	OnBombPicked.Broadcast(Character);
}

bool AMyC4::PostUse(AMyCharacter* Character)
{
	const auto& Result = Super::PostUse(Character);

	if (!IsPlanted())
	{
		return TryPlant(Character) && Result;
	}
	else
	{
		return Result;
	}
}

void AMyC4::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

bool AMyC4::TryDefuse(AMyCharacter* Character)
{
	if (IsDefused() || IsExploded())
	{
		LOG_FUNC(LogTemp, Warning, "Bomb is exploded or defused");
		return false;
	}

	if (DefusingCharacter.IsValid() && DefusingCharacter != Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Another character is defusing"));
		return false;
	}

	DefusingCharacter = Character;

	if (!IsPlanted())
	{
		LOG_FUNC(LogTemp, Warning, "Bomb is not planted");
		return false;
	}

	if (!IsDefusable(false))
	{
		LOG_FUNC(LogTemp, Warning, "Bomb is not defusable");
		return false;
	}

	Character->GetMovementComponent()->StopMovementImmediately();
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	UE_LOG(LogTemp, Warning, TEXT("Bomb defusing"));

	LOG_FUNC(LogTemp, Warning, "Establish Defusing handle");
	if (OnBombDefusingHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(OnBombDefusingHandle);
	}

	GetWorldTimerManager().SetTimer
	(
	 OnBombDefusingHandle,
	 this,
	 &AMyC4::OnBombDefusedImpl,
	 FullDefusingTime,
	 false
	);

	DefuserAttackHandle = DefusingCharacter->BindOnAttackStarted(this, &AMyC4::InteractInterrupted);

	DefuserOnInteractInterruptedHandle = DefusingCharacter->BindOnInteractInterrupted(
		this, &AMyC4::InteractInterrupted);

	
	SetDefusing(Character);

	return true;
}

void AMyC4::DefuseInterrupted()
{
	if (IsDefusing() && GetDefusingRatio() < 1.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Defusing interrupted"));

		DefusingCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		DefusingCharacter->UnbindOnAttackStarted(DefuserAttackHandle);
		DefusingCharacter->UnbindOnInteractInterrupted(DefuserOnInteractInterruptedHandle);
		GetWorldTimerManager().ClearTimer(OnBombDefusingHandle);
		SetDefusing(nullptr);
	}
}

void AMyC4::InteractInterrupted()
{
	Super::InteractInterrupted();

	DefuseInterrupted();
}

void AMyC4::PlantInterrupted()
{
	if (IsPlanting() && GetPlantingRatio() < 1.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Planting interrupted"));

		GetItemOwner()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		GetItemOwner()->UnbindOnAttackStarted(PlanterAttackHandle);
		GetWorldTimerManager().ClearTimer(OnBombPlantingHandle);
		SetPlanting(false);
	}
}

void AMyC4::UseInterrupted()
{
	Super::UseInterrupted();

	PlantInterrupted();
}

void AMyC4::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsPlanting())
	{
		PlantingTime += DeltaSeconds;
	}

	if (IsDefusing())
	{
		DefusingTime += DeltaSeconds;
	}

	if (IsPlanted())
	{
		Elapsed += DeltaSeconds;
	}
}

void AMyC4::SetDefusing(AMyCharacter* Character)
{
	DefusingCharacter = Character;
	DefusingTime = 0.f;

	if (DefusingCharacter.IsValid())
	{
		SetState(EMyBombState::Defusing);
	}
	else
	{
		SetState(EMyBombState::Planted);
	}
}

void AMyC4::SetPlanting(const bool NewPlanting)
{
	PlantingTime = 0.f;

	if (NewPlanting)
	{
		SetState(EMyBombState::Planting);
	}
	else
	{
		SetState(EMyBombState::Idle);
	}
}
