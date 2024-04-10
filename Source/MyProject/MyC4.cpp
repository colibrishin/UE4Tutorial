// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyC4.h"

// Sets default values
AMyC4::AMyC4()
	: IsPlanted(false),
	  IsPlanting(false),
	  IsDefusing(false),
	  IsExploded(false),
	  PlantingTime(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_C4(TEXT("StaticMesh'/Game/Models/C4-1.C4-1'"));

	if (SM_C4.Succeeded())
	{
		GetMesh()->SetStaticMesh(SM_C4.Object);
	}
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

	if (GetItemOwner()->GetState() != Planting)
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner is not planting"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Bomb planted"));

	IsPlanting   = false;
	IsPlanted    = true;
	PlantingTime = 0.f;

	OnBombPlantedDelegate.Broadcast();

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

	if (DefusingCharacter->GetState() != Defusing)
	{
		return;
	}

	IsDefusing = false;
	IsPlanted  = false;

	DefusingCharacter = nullptr;

	GetWorldTimerManager().ClearTimer(OnBombTicking);
	GetWorldTimerManager().ClearTimer(OnBombDefusing);
}

bool AMyC4::InteractImpl(AMyCharacter* Character)
{
	return Super::InteractImpl(Character);
}

bool AMyC4::UseImpl(AMyCharacter* Character)
{
	FHitResult                  HitResult;
	const FCollisionQueryParams Params{NAME_None, false, this};

	const auto& Result = GetWorld()->OverlapAnyTestByChannel
	(
		GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_EngineTraceChannel6,
		FCollisionShape::MakeSphere(100.f),
		Params
	);

	if (!Result)
	{
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Bomb plant check passed"));

	if (IsPlanting || IsExploded)
	{
		return false;
	}

	// todo: need to know character is defusable
	if (IsPlanted && !IsDefusing)
	{
		IsDefusing = true;

		UE_LOG(LogTemp, Warning, TEXT("Bomb defusing"));

		GetWorldTimerManager().SetTimer(
		OnBombDefusing,
			this,
			&AMyC4::OnBombDefusedImpl,
			FullDefusingTime,
			false);

		return true;
	}
	else
	{
		IsPlanting = true;

		UE_LOG(LogTemp, Warning, TEXT("Bomb planting"));

		GetWorldTimerManager().SetTimer(
			OnBombPlanted, 
			this,
			&AMyC4::OnBombPlantedImpl,
			FullPlantingTime,
			false);

		return true;
	}

	return false;
}

void AMyC4::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsPlanted)
	{
		PlantingTime += DeltaSeconds;
	}
}

void AMyC4::SetDefusingCharacter(AMyCharacter* Character)
{
	DefusingCharacter = Character;
}


