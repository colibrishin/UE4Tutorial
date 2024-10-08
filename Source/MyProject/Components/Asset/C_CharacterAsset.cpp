// Fill out your copyright notice in the Description page of Project Settings.


#include "C_CharacterAsset.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"

#include "GameFramework/SpringArmComponent.h"

#include "MyProject/MyPlayerState.h"
#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Components/C_Health.h"
#include "MyProject/DataAsset/DA_Character.h"


// Sets default values for this component's properties
UC_CharacterAsset::UC_CharacterAsset()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UC_CharacterAsset::ApplyAsset()
{
	Super::ApplyAsset();

	if (const UDA_Character* CharacterAsset = GetAsset<UDA_Character>())
	{
		AA_Character* Character = Cast<AA_Character>( GetOwner() );

		if (Character == nullptr)
		{
			Character = Cast<AA_Character>( GetOuter() );
		}
		check( Character != nullptr );

		Character->GetMesh()->SetSkeletalMesh( CharacterAsset->GetSkeletalMesh() );
		Character->GetMesh()->SetRelativeLocation( CharacterAsset->GetMeshOffset() );
		Character->GetMesh()->SetRelativeRotation( CharacterAsset->GetMeshRotation() );
		Character->GetMesh()->SetOwnerNoSee( CharacterAsset->GetOwnerNoSee() );
		Character->GetMesh()->SetGenerateOverlapEvents( CharacterAsset->GetMeshOverlapEvent() );
		Character->GetMesh()->SetAnimInstanceClass( CharacterAsset->GetAnimInstance() );

		Character->GetArmMesh()->SetSkeletalMesh( CharacterAsset->GetArmMesh() );
		Character->GetArmMesh()->SetAnimInstanceClass( CharacterAsset->GetArmAnimInstance() );

		Character->GetArmMesh()->SetRelativeLocation( CharacterAsset->GetArmOffset() );
		Character->GetArmMesh()->SetRelativeRotation( CharacterAsset->GetArmRotation() );

		if ( !Character->GetComponentByClass<UCapsuleComponent>() )
		{
			Character->GetArmMesh()->SetWorldScale3D( CharacterAsset->GetSize() );
		}

		if ( USpringArmComponent* SpringArmComponent = Character->GetComponentByClass<USpringArmComponent>() )
		{
			SpringArmComponent->SetRelativeLocation( CharacterAsset->GetCameraOffset() );
			SpringArmComponent->SetRelativeRotation( CharacterAsset->GetCameraRotation() );
			SpringArmComponent->TargetOffset = CharacterAsset->GetCameraArmTargetOffset();
			SpringArmComponent->SocketOffset = CharacterAsset->GetCameraArmSocketOffset();
			SpringArmComponent->TargetArmLength = CharacterAsset->GetCameraArmLength();
			SpringArmComponent->ProbeSize = CharacterAsset->GetCollisionProbeSize();
		}

		if ( const AMyPlayerState* PlayerState = Character->GetPlayerState<AMyPlayerState>() )
		{
			if ( UC_Health* HealthComponent = PlayerState->GetHealthComponent() )
			{
				HealthComponent->MaxHealth = CharacterAsset->GetMaxHealth();
			}
		}

		// note: Replication Mesh offset setup
		Character->CacheInitialMeshOffset(
			CharacterAsset->GetMeshOffset() , CharacterAsset->GetMeshRotation() );
	}
}


// Called when the game starts
void UC_CharacterAsset::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
