// Fill out your copyright notice in the Description page of Project Settings.


#include "C_JumpRevert.h"

#include "C_JumpCheckpoint.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationSystem.h"
#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Jump/Frameworks/GS_Jump.h"
#include "MyProject/Jump/Frameworks/PC_Jump.h"
#include "MyProject/Private/Utilities.hpp"

// Sets default values for this component's properties
UC_JumpRevert::UC_JumpRevert()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Jump( TEXT( "/Script/EnhancedInput.InputMappingContext'/Game/Blueprints/Jump/Inputs/IMC_Jump.IMC_Jump'" ) );
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_JumpRevert( TEXT( "/Script/EnhancedInput.InputAction'/Game/Blueprints/Jump/Inputs/IA_JumpRevert.IA_JumpRevert'" ) );

	// ...
	if (IMC_Jump.Succeeded())
	{
		InputMappingContext = IMC_Jump.Object;
	}

	if (IA_JumpRevert.Succeeded())
	{
		RevertAction = IA_JumpRevert.Object;
	}
}

void UC_JumpRevert::Revert()
{
	if (const AGS_Jump* GameState = GetWorld()->GetGameState<AGS_Jump>())
	{
		if (const UC_JumpCheckpoint* Checkpoint = GameState->GetLastCheckpoint())
		{
			const FVector MeshFindingPivot = Checkpoint->GetOwner()->GetActorLocation();
			FNavLocation OutLocation;

			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		
			NavSystem->ProjectPointToNavigation(MeshFindingPivot, OutLocation, FVector::ZeroVector, (FNavAgentProperties*)nullptr);
			GetOwner()->SetActorLocation(OutLocation.Location, false, nullptr, ETeleportType::ResetPhysics);
			
		}
		else
		{
			LOG_FUNC(LogTemp, Warning, "No Last checkpoint has been set!");
		}
	}
}


// Called when the game starts
void UC_JumpRevert::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (const AA_Character* Character = Cast<AA_Character>(GetOwner()))
	{
		if (const APC_Jump* PlayerController = Cast<APC_Jump>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}

			if (UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(Character->InputComponent))
			{
				InputComponent->BindAction(RevertAction, ETriggerEvent::Started, this, &UC_JumpRevert::Revert);
			}
		}
	}
}


// Called every frame
void UC_JumpRevert::TickComponent(float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	// ...
}

