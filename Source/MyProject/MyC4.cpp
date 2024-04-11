// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyC4.h"

#include "ConstantFVector.hpp"
#include "DrawDebugHelpers.h"

#include "Components/BoxComponent.h"

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

	DrawDebugLine
	(
		GetWorld(),
		GetActorLocation(),
		HitResult.Location,
		GroundResult ? FColor::Green : FColor::Red,
		false,
		1.f
	);

	return !IsPlanted && !IsExploded && GroundResult && OverlapResult;
}

bool AMyC4::IsDefusable() const
{
	return IsPlanted && !IsExploded && !IsDefused;
}

bool AMyC4::Interact(AMyCharacter* Character)
{
	if (!IsDefusable())
	{
		if (IsExploded || IsDefused)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bomb is exploded or defused"));
			return false;
		}

		return Super::Interact(Character);
	}

	return TryDefuse(Character);
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

	UE_LOG(LogTemp, Warning, TEXT("Bomb Exploded"));
	GetWorldTimerManager().ClearTimer(OnBombTicking);
}

void AMyC4::OnBombPlantedImpl()
{
	if (!IsValid(GetItemOwner()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner is not valid"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Bomb planted"));

	FHitResult  HitResult;
	const auto& GroundResult = IsPlantable(OUT HitResult);

	if (!GroundResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bomb is not on the ground"));
		return;
	}

	IsPlanting   = false;
	IsPlanted    = true;
	PlantingTime = 0.f;

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

bool AMyC4::TryDefuse(AMyCharacter* Character)
{
	if (IsDefused || IsExploded)
	{
		return false;
	}

	if (IsDefusable())
	{
		// todo: need to know character is defusable
		SetDefusing(true, Character);

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
		}

		return true;
	}

	return false;
}

bool AMyC4::Use(AMyCharacter* Character)
{
	if (!IsPlanted)
	{
		FHitResult HitResult;
		if (!IsPlantable(OUT HitResult))
		{
			UE_LOG(LogTemp, Warning, TEXT("Bomb is not plantable"));
			return false;
		}

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
		}

		return true;
	}

	return false;
}

void AMyC4::Recycle()
{
	UE_LOG(LogTemp, Warning, TEXT("Interrupt caught"));

	if (IsPlanting && GetPlantingRatio() < 1.f)
	{
		GetWorldTimerManager().ClearTimer(OnBombPlanted);
		SetPlanting(false);
	}

	if (IsDefusing && GetDefusingRatio() < 1.f)
	{
		GetWorldTimerManager().ClearTimer(OnBombDefusing);
		SetDefusing(false, nullptr);
	}
}

void AMyC4::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsPlanted)
	{
		PlantingTime += DeltaSeconds;
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
