// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyC4.h"

#include "DrawDebugHelpers.h"
#include "MyCharacter.h"
#include "MyPlayerState.h"
#include "Components/BoxComponent.h"
#include "Engine/OverlapResult.h"
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

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_C4(TEXT("StaticMesh'/Game/Models/C4-1.C4-1'"));

	if (SM_C4.Succeeded())
	{
		GetSkeletalMeshComponent()->SetSkeletalMesh(SM_C4.Object);
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
		GetActorLocation() + (FVector::DownVector * 1000.f),
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

	const auto& DefuserCheck = GetWorld()->OverlapMultiByChannel
	(
		OverlapResult,
		GetActorLocation(),
		FQuat::Identity,
		ECC_GameTraceChannel1,
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

void AMyC4::Server_Interact_Implementation(AMyCharacter* Character)
{
	Super::Server_Interact_Implementation(Character);

	// Delegating Defusing condition check to server, If it is possible server will answer with RPC.
	TryDefuse(Character);
}

void AMyC4::Server_Use_Implementation(AMyCharacter* Character)
{
	Super::Server_Use_Implementation(Character);

	// Delegating Planting condition check to server, If it is possible server will answer with RPC.
	TryPlant(Character);
}

void AMyC4::Client_TryPlanting_Implementation(AMyCharacter* Character)
{
	// Wrapper function for client RPC
	PresetPlant(Character);
}

void AMyC4::Client_TryDefusing_Implementation(AMyCharacter* Character)
{
	// Wrapper function for client RPC
	PresetDefuse(Character);
}

void AMyC4::PresetPlant(AMyCharacter* Character)
{
	// Planting condition check is okay, set the status and event for plant.
	// Note that, Preset series function will be executed same code for server and client.
	Character->GetMovementComponent()->StopMovementImmediately();
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetItemOwner()->OnAttackStarted.AddUniqueDynamic(this, &AMyC4::Server_UseInterrupted);
	
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
}

void AMyC4::PresetDefuse(AMyCharacter* Character)
{
	// Defusing condition check is okay, set the status and event for defusing.
	// Note that, Preset series function will be executed same code for server and client.
	Character->GetMovementComponent()->StopMovementImmediately();
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	UE_LOG(LogTemp, Warning, TEXT("Bomb defusing"));

	if (HasAuthority())
	{
		// Attach to defusing player so we can do client replication.
		SetOwner(DefusingCharacter.Get());
	}
	
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

	SetDefusing(Character);

	DefusingCharacter->OnAttackStarted.AddDynamic(this, &AMyC4::Server_InteractInterrupted);
	DefusingCharacter->OnInteractInterrupted.AddDynamic(this, &AMyC4::Server_InteractInterrupted);
}

void AMyC4::Client_UnsetDefuse_Implementation(AMyCharacter* Character)
{
	// Remove observer and constraint on client side.
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	Character->OnAttackStarted.RemoveDynamic(this, &AMyC4::Server_InteractInterrupted);
	Character->OnInteractInterrupted.RemoveDynamic(this, &AMyC4::Server_InteractInterrupted);
	GetWorldTimerManager().ClearTimer(OnBombDefusingHandle);
}

void AMyC4::Client_UnsetPlanting_Implementation(AMyCharacter* Character)
{
	// Remove observer and constraint on server side.
	GetItemOwner()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetItemOwner()->OnAttackStarted.RemoveDynamic(this, &AMyC4::Server_UseInterrupted);
	GetWorldTimerManager().ClearTimer(OnBombPlantingHandle);
}

void AMyC4::Multi_StartBombTick_Implementation()
{
	// Execute bomb tick timer for every clients.
	GetWorldTimerManager().SetTimer
			(
			 OnBombExplodedHandle ,
			 this ,
			 &AMyC4::OnBombExplodedImpl ,
			 FullExplodingTime ,
			 false
			);

	// Freeze the bomb position!
	GetSkeletalMeshComponent()->SetSimulatePhysics(false);
}

void AMyC4::OnBombExplodedImpl()
{
	// Trigger by server.
	if (HasAuthority())
	{
		if (IsDefusing())
		{
			// Remove observer and constraint.
			DefusingCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			DefusingCharacter->OnAttackStarted.RemoveDynamic(this, &AMyC4::Server_InteractInterrupted);
			DefusingCharacter->OnInteractInterrupted.RemoveDynamic(this, &AMyC4::Server_InteractInterrupted);

			// Also unset the client side defusing status of defusing character.
			Client_UnsetDefuse(DefusingCharacter.Get());

			// Detach from defuser.
			SetOwner(nullptr);
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

		// Remove observer and constraint on server side.
		GetItemOwner()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		GetItemOwner()->OnAttackStarted.RemoveDynamic(this, &AMyC4::Server_UseInterrupted);
		GetWorldTimerManager().ClearTimer(OnBombPlantingHandle);

		// Remove observer and constraint on client side.
		Client_UnsetPlanting(GetItemOwner());

		Server_Drop();

		// Freeze the bomb position!
		GetSkeletalMeshComponent()->SetSimulatePhysics(false);

		// Execute bomb tick timer for every clients.
		Multi_StartBombTick();

		SetOwner(nullptr);
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
		DefusingCharacter->OnAttackStarted.RemoveDynamic(this, &AMyC4::Server_InteractInterrupted);
		DefusingCharacter->OnInteractInterrupted.RemoveDynamic(this, &AMyC4::Server_InteractInterrupted);

		Client_UnsetDefuse(DefusingCharacter.Get());

		SetState(EMyBombState::Defused);
		LOG_FUNC(LogTemp, Warning, "Bomb defused");
		GetWorldTimerManager().ClearTimer(OnBombExplodedHandle);
		GetWorldTimerManager().ClearTimer(OnBombDefusingHandle);

		// Detach from defuser.
		SetOwner(nullptr);
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

		return Super::PreInteract(Character) && CTPickPlantGuard(Character);
	}
	else
	{
		return TDefuseGuard(Character) && Super::PreInteract(Character);
	}
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

bool AMyC4::TryAttachItem(AMyCharacter* Character)
{
	if (Super::TryAttachItem(Character))
	{
		SetOwner(Character);
		return true;
	}

	return false;
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

bool AMyC4::TryPlant(AMyCharacter* Character)
{
	// Check condition for planting from server-side.
	if (IsPlanted())
	{
		return false;
	}

	if (!IsPlantable(false))
	{
		return false;
	}

	// Set plant status in server side.
	PresetPlant(Character);
	// Set plant status in client side.
	Client_TryPlanting(Character);

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

	if (!IsPlanted())
	{
		LOG_FUNC(LogTemp, Warning, "Bomb is not planted");
		return false;
	}

	DefusingCharacter = Character;

	if (!IsDefusable(false))
	{
		LOG_FUNC(LogTemp, Warning, "Bomb is not defusable");
		DefusingCharacter = nullptr;
		return false;
	}

	// Set the defusing status in server side.
	PresetDefuse(Character);
	// Set the defusing status in client side.
	Client_TryDefusing(Character);

	// Even if it is success, keep the post interact failing to not pick up.
	return false;
}

void AMyC4::DefuseInterrupted()
{
	if (IsDefusing() && GetDefusingRatio() < 1.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Defusing interrupted"));

		DefusingCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		DefusingCharacter->OnAttackStarted.RemoveDynamic(this, &AMyC4::Server_InteractInterrupted);
		DefusingCharacter->OnInteractInterrupted.RemoveDynamic(this, &AMyC4::Server_InteractInterrupted);

		GetWorldTimerManager().ClearTimer(OnBombDefusingHandle);

		SetDefusing(nullptr);
		SetOwner(nullptr);
	}
}

void AMyC4::Server_InteractInterrupted_Implementation()
{
	Super::Server_InteractInterrupted_Implementation();
	DefuseInterrupted();
}

void AMyC4::PlantInterrupted()
{
	if (IsPlanting() && GetPlantingRatio() < 1.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Planting interrupted"));

		GetItemOwner()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		GetItemOwner()->OnAttackStarted.RemoveDynamic(this, &AMyC4::Server_InteractInterrupted);
		GetWorldTimerManager().ClearTimer(OnBombPlantingHandle);
		SetPlanting(false);
	}
}

void AMyC4::Server_UseInterrupted_Implementation()
{
	Super::Server_UseInterrupted_Implementation();
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
