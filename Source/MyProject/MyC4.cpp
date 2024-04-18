// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyC4.h"

#include "ConstantFVector.hpp"
#include "DrawDebugHelpers.h"
#include "MyBombProgressWidget.h"
#include "MyCharacter.h"
#include "MyInGameHUD.h"
#include "MyInventoryComponent.h"
#include "MyPlayerState.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

#include "GameFramework/HUD.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AMyC4::AMyC4()
	: IsPlanted(false),
	  IsPlanting(false),
	  IsDefusing(false),
	  IsExploded(false),
	  PlantingTime(0),
      DefusingTime(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_C4(TEXT("StaticMesh'/Game/Models/C4-1.C4-1'"));

	if (SM_C4.Succeeded())
	{
		GetMesh()->SetStaticMesh(SM_C4.Object);
	}
}

bool AMyC4::IsPlantable(OUT FHitResult& OutResult) const
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

	float Speed = 1.f;

	if (IsValid(GetItemOwner()))
	{
		Speed = GetItemOwner()->GetVelocity().Size();
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

	return !IsPlanted && !IsExploded && GroundResult && OverlapResult && (Speed == 0.f);
}

bool AMyC4::IsDefusable() const
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

	float Speed = 1.f;

	if (DefusingCharacter.IsValid())
	{
		Speed = DefusingCharacter->GetVelocity().Size();
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

	return IsPlanted && !IsExploded && !IsDefused && IsDefuserNearby && (Speed == 0.f);
}

// Called when the game starts or when spawned
void AMyC4::BeginPlay()
{
	Super::BeginPlay();
}

void AMyC4::OnBombTickingImpl()
{
	IsPlanted = false;
	IsExploded = true;

	if (IsDefusing)
	{
		SetDefusing(false, nullptr);
	}

	UE_LOG(LogTemp, Warning, TEXT("Bomb Exploded"));
	GetWorldTimerManager().ClearTimer(OnBombTicking);
}

void AMyC4::OnBombPlantedImpl()
{
	if (!IsValid(GetItemOwner()))
	{
		LOG_FUNC(LogTemp, Warning, "Item owner is not valid");
		PlantingTime = 0.f;
		return;
	}

	FHitResult  HitResult;
	const auto& GroundResult = IsPlantable(OUT HitResult);

	if (!GroundResult)
	{
		LOG_FUNC(LogTemp, Warning, "Bomb is not plantable");
		PlantingTime = 0.f;
		return;
	}

	LOG_FUNC(LogTemp, Warning, "Bomb planted");

	IsPlanting   = false;
	IsPlanted    = true;
	Elapsed = 0.f;

	OnBombPlantedDelegate.Broadcast();

	Drop();

	GetWorldTimerManager().ClearTimer(OnBombPlanted);

	GetWorldTimerManager().SetTimer(
		OnBombTicking,
		this,
		&AMyC4::OnBombTickingImpl,
		FullExplodingTime,
		false);
}

void AMyC4::OnBombDefusedImpl()
{
	if (!DefusingCharacter.IsValid())
	{
		DefusingTime = 0.f;
		return;
	}

	// Second chance, preventing from not InteractInterrupted
	if (!IsDefusable())
	{
		DefusingTime = 0.f;
		return;
	}

	IsDefusing = false;
	IsPlanted  = false;
	IsDefused  = true;

	DefusingCharacter = nullptr;

	UE_LOG(LogTemp, Warning, TEXT("Bomb defused"));
	GetWorldTimerManager().ClearTimer(OnBombTicking);
	GetWorldTimerManager().ClearTimer(OnBombDefusing);
}

bool AMyC4::PreInteract(AMyCharacter* Character)
{
	if (!IsPlanted)
	{
		if (IsExploded || IsDefused)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bomb is exploded or defused"));
			return false;
		}
	}

	if (IsPlanted)
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
	}

	return true;
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
	}

	return true;
}

bool AMyC4::PostInteract(AMyCharacter* Character)
{
	if (IsPlanted)
	{
		return TDefuseGuard(Character) && TryDefuse(Character);
	}
	else
	{
		return CTPickPlantGuard(Character) && Super::PostInteract(Character);
	}
}

bool AMyC4::TryAttachItem(const AMyCharacter* Character)
{
	return Character->GetInventory()->TryAddItem(this) && Super::TryAttachItem(Character);
}

bool AMyC4::PreUse(AMyCharacter* Character)
{
	if (!IsPlanted)
	{
		if (IsExploded || IsDefused)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bomb is exploded or defused"));
			return false;
		}

		return CTPickPlantGuard(Character);
	}

	return Super::PreUse(Character);
}

bool AMyC4::PostUse(AMyCharacter* Character)
{
	FHitResult HitResult;
	if (!IsPlanted && IsPlantable(HitResult))
	{
		SetPlanting(true);

		UE_LOG(LogTemp, Warning, TEXT("Bomb planting"));

		if (!OnBombPlanted.IsValid())
		{
			GetWorldTimerManager().SetTimer(
				OnBombPlanted, 
				this,
				&AMyC4::OnBombPlantedImpl,
				FullPlantingTime,
				false);

			if (IsValid(GetItemOwner()))
			{
				ShowBombProgressWidget(GetItemOwner());	
			}
		}
	}

	return Super::PostUse(Character);
}

void AMyC4::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

bool AMyC4::TryDefuse(AMyCharacter* Character)
{
	if (IsDefused || IsExploded)
	{
		return false;
	}

	if (DefusingCharacter.IsValid() && DefusingCharacter != Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Another character is defusing"));
		return false;
	}

	// todo: need to know character is defusable
	SetDefusing(true, Character);

	if (IsDefusable())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bomb defusing"));

		if (!OnBombDefusing.IsValid())
		{
			GetWorldTimerManager().SetTimer
			(
			 OnBombDefusing,
			 this,
			 &AMyC4::OnBombDefusedImpl,
			 FullDefusingTime,
			 false
			);

			if (IsValid(GetDefusingCharacter()))
			{
				ShowBombProgressWidget(GetDefusingCharacter());
			}

			DefuserOnInteractInterruptedHandle = DefusingCharacter->BindOnInteractInterrupted(
				this, &AMyC4::InteractInterrupted);
		}

		return true;
	}
	else
	{
		SetDefusing(false, nullptr);
	}

	return false;
}

void AMyC4::InteractInterrupted()
{
	Super::InteractInterrupted();

	if (IsDefusing && GetDefusingRatio() < 1.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Defusing interrupted"));
		GetWorldTimerManager().ClearTimer(OnBombDefusing);

		if (IsValid(GetDefusingCharacter()))
		{
			HideBombProgressWidget(GetDefusingCharacter());
		}
		
		DefusingCharacter->UnbindOnInteractInterrupted(DefuserOnInteractInterruptedHandle);
		SetDefusing(false, nullptr);
	}
}

void AMyC4::UseInterrupted()
{
	Super::UseInterrupted();

	if (IsPlanting && GetPlantingRatio() < 1.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Planting interrupted"));
		GetWorldTimerManager().ClearTimer(OnBombPlanted);

		if (IsValid(GetItemOwner()))
		{
			HideBombProgressWidget(GetItemOwner());	
		}

		SetPlanting(false);
	}
}

void AMyC4::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsPlanting)
	{
		PlantingTime += DeltaSeconds;
	}

	if (IsDefusing)
	{
		DefusingTime += DeltaSeconds;
	}

	if (IsPlanted)
	{
		Elapsed += DeltaSeconds;
	}
}

void AMyC4::SetDefusing(const bool NewDefusing, AMyCharacter* Character)
{
	IsDefusing = NewDefusing;
	DefusingCharacter = Character;

	if (!IsDefusing)
	{
		DefusingTime = 0.f;
	}
}

void AMyC4::SetPlanting(const bool NewPlanting)
{
	IsPlanting = NewPlanting;

	if (!IsPlanting)
	{
		PlantingTime = 0.f;
	}
}

void AMyC4::ShowBombProgressWidget(const AMyCharacter* Character) const
{
	if (!IsValid(Character))
	{
		return;
	}

	if (Character != GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		return;
	}

	const auto& PlayerController = Cast<APlayerController>(Character->GetController());

	if (!IsValid(PlayerController))
	{
		return;
	}

	const auto& HUD = Cast<AMyInGameHUD>(PlayerController->GetHUD());

	if (!IsValid(HUD))
	{
		return;
	}

	HUD->BindBomb(this);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Bomb widget updated"));
}

void AMyC4::HideBombProgressWidget(const AMyCharacter* Character) const
{
	if (!IsValid(Character))
	{
		return;
	}

	if (Character != GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		return;
	}

	const auto& PlayerController = Cast<APlayerController>(Character->GetController());

	if (!IsValid(PlayerController))
	{
		return;
	}

	const auto& HUD = Cast<AMyInGameHUD>(PlayerController->GetHUD());

	if (!IsValid(HUD))
	{
		return;
	}

	HUD->BindBomb(nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Bomb widget updated"));
}
