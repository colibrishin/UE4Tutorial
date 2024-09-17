// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Character.h"

#include "MyProject/MyPlayerState.h"
#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Components/C_Health.h"
#include "MyProject/DataAsset/DA_Character.h"


// Sets default values for this component's properties
UC_Character::UC_Character()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UC_Character::ApplyAsset()
{
	Super::ApplyAsset();

	if (const UDA_Character* CharacterAsset = GetAsset<UDA_Character>())
	{
		if (const AA_Character* Character = Cast<AA_Character>(GetOwner()))
		{
			Character->GetMesh()->SetAnimInstanceClass(CharacterAsset->GetAnimInstance());
			Character->GetArmMesh()->SetSkeletalMesh(CharacterAsset->GetArmMesh());
			Character->GetArmMesh()->SetAnimInstanceClass(CharacterAsset->GetArmAnimInstance());

			if (const AMyPlayerState* PlayerState = Character->GetPlayerState<AMyPlayerState>())
			{
				if (UC_Health* HealthComponent = PlayerState->GetHealthComponent())
				{
					HealthComponent->MaxHealth = CharacterAsset->GetMaxHealth();
				}
			}
		}
	}
}


// Called when the game starts
void UC_Character::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
