// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_Jump.h"

#include "GS_Jump.h"
#include "HD_Jump.h"
#include "PC_Jump.h"

#include "Kismet/GameplayStatics.h"

#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Components/Asset/C_CharacterAsset.h"
#include "MyProject/Jump/Actors/A_JumpCharacter.h"


// Sets default values
AGM_Jump::AGM_Jump()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGM_Jump::BeginPlay()
{
	Super::BeginPlay();
	
}

APawn* AGM_Jump::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	const FTransform Transform{FQuat::Identity, StartSpot->GetActorLocation(), FVector::OneVector};

	AA_Character* Character = GetWorld()->SpawnActorDeferred<AA_Character>(
		AA_JumpCharacter::StaticClass(),
		Transform,
		NewPlayer,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
		ESpawnActorScaleMethod::OverrideRootScale);

	Character->GetAssetComponent()->SetID(CharacterAssetID);
	Character->FetchAsset();
	UGameplayStatics::FinishSpawningActor(Character, Transform);
	return Character;
}

// Called every frame
void AGM_Jump::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

