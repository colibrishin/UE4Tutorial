


#include "A_Character.h"

#include "A_Collectable.h"
#include "EnhancedInputComponent.h"

#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Private/Utilities.hpp"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"

#include "GameFramework/SpringArmComponent.h"

#include "MyProject/MyPlayerState.h"
#include "MyProject/Components/Asset/C_CharacterAsset.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogCharacter);

const FName AA_Character::LeftHandSocketName( TEXT( "Muzzle_02" ) );
const FName AA_Character::RightHandSocketName( TEXT( "Muzzle_01" ) );
const FName AA_Character::HeadSocketName( TEXT( "head_socket" ) );
const FName AA_Character::ChestSocketName( TEXT( "Chest" ) );

// Sets default values
AA_Character::AA_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	if (static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Movement(
		TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Blueprints/Inputs/InputContext/IMC_Movement.IMC_Movement'"));
		IMC_Movement.Succeeded())
	{
		InputMapping = IMC_Movement.Object;
	}

	if (static ConstructorHelpers::FObjectFinder<UInputAction> IA_Move(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Inputs/IA_Move.IA_Move'"));
		IA_Move.Succeeded())
	{
		MoveAction = IA_Move.Object;
	}

	if (static ConstructorHelpers::FObjectFinder<UInputAction> IA_Look(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Inputs/IA_Look.IA_Look'"));
		IA_Look.Succeeded())
	{
		LookAction = IA_Look.Object;
	}

	if (static ConstructorHelpers::FObjectFinder<UInputAction> IA_Jump(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Inputs/IA_Jump.IA_Jump'"));
		IA_Jump.Succeeded())
	{
		JumpAction = IA_Jump.Object;
	}
	
	ArmMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
	AssetComponent = CreateDefaultSubobject<UC_CharacterAsset>(TEXT("AssetComponent"));
	Camera1P = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));

	GetMesh()->SetupAttachment(GetCapsuleComponent());
	ArmMeshComponent->SetupAttachment(GetCapsuleComponent());
	SpringArmComponent->SetupAttachment(GetCapsuleComponent());
	Camera1P->SetupAttachment(SpringArmComponent);

	SpringArmComponent->bDoCollisionTest = true;
	SpringArmComponent->ProbeSize = 100.f;
	SpringArmComponent->TargetArmLength = 0.f;
	SpringArmComponent->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetCastShadow(true);
	ArmMeshComponent->SetOnlyOwnerSee(true);
	ArmMeshComponent->SetCastShadow(false);
	
	AssetComponent->SetNetAddressable();
	AssetComponent->OnAssetIDSet.AddUObject(
		this, &AA_Character::FetchAsset);

	GetCapsuleComponent()->InitCapsuleSize(88.f, 88.f);
	OnHandChanged.AddUObject(this, &AA_Character::ClientDuplicateHand);
}

// Called when the game starts or when spawned
void AA_Character::BeginPlay()
{
	Super::BeginPlay();

	if ( const APlayerController* PlayerController = Cast<APlayerController>( Controller );
		 PlayerController && PlayerController == GetWorld()->GetFirstPlayerController())
	{
		if ( UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( PlayerController->GetLocalPlayer() ) )
		{
			Subsystem->AddMappingContext( InputMapping , 0 );
		}
	}
}

void AA_Character::PickUp(UC_PickUp* InPickUp)
{
	IPickingUp::PickUp(InPickUp);

	if (GetNetMode() == NM_Client)
	{
		return;
	}

	if (!Cast<AA_Collectable>(InPickUp->GetOwner())->IsDummy())
	{
		if (bHandBusy)
		{
			LOG_FUNC_PRINTF( LogCharacter , Log , "Hand is busy, drop item %s back", *InPickUp->GetOwner()->GetName());
			InPickUp->OnObjectDrop.Broadcast( this );
			return;
		}
		
		if (InPickUp->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		RightHandSocketName
		) )
		{
			bHandBusy = true;
			OnHandChanged.Broadcast(Hand, InPickUp);
			Hand = InPickUp;
		}
		else
		{
			ensureAlwaysMsgf( false, TEXT("%s; Unable to attach item %s to the hand;") , *GetName() , *InPickUp->GetOwner()->GetName());
		}
	}
	else
	{
		if (!InPickUp->AttachToComponent(
			ArmMeshComponent,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			RightHandSocketName
		))
		{
			ensureAlwaysMsgf( false, TEXT("%s; Unable to attach dummy item %s to the arm;") , *GetName() , *InPickUp->GetOwner()->GetName());
		}
	}
}

void AA_Character::Drop(UC_PickUp* InPickUp)
{
	IPickingUp::Drop(InPickUp);
	
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	// If Item is attached to character;
	if (UC_PickUp* PickUp = InPickUp->GetOwner()->GetComponentByClass<UC_PickUp>();
		 PickUp && PickUp->GetAttachParentActor() == this)
	{
		const AA_Collectable* Collectable = Cast<AA_Collectable>(InPickUp->GetOwner());
		ensure(Collectable);

		if (!Collectable->IsDummy())
		{
			PickUp->DetachFromComponent( FDetachmentTransformRules::KeepWorldTransform );
			bHandBusy = false;
			OnHandChanged.Broadcast(PickUp, nullptr);
			Hand = nullptr;
		}
		else
		{
			HandArm->Destroy(true);
		}
	}
}

void AA_Character::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	// add movement 
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void AA_Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	// add yaw and pitch input to controller
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(-LookAxisVector.Y);
}

// Called every frame
void AA_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AA_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AA_Character::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AA_Character::Look);
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AA_Character::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AA_Character::StopJumping);
	}
}

void AA_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AA_Character, bHandBusy);
	DOREPLIFETIME(AA_Character, Hand);
	DOREPLIFETIME(AA_Character, AssetComponent);
	DOREPLIFETIME_CONDITION(AA_Character, HandArm, COND_OwnerOnly);
}

void AA_Character::OnRep_Hand(UC_PickUp* InOldHand) const
{
	OnHandChanged.Broadcast(InOldHand, Hand);
}

void AA_Character::ClientDuplicateHand(UC_PickUp* InOldHand, UC_PickUp* InNewHand)
{
	if (GetNetMode() != NM_Client)
	{
		if (InOldHand)
		{
			HandArm->GetPickUpComponent()->OnObjectDrop.Broadcast(this);
		}

		if (InNewHand)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetController();
			SpawnParameters.Template = InNewHand->GetOwner();
		
			HandArm = GetWorld()->SpawnActor<AA_Collectable>(InNewHand->GetOwner()->GetClass(), SpawnParameters);
			HandArm->SetReplicates(true);
			HandArm->bOnlyRelevantToOwner = true;
			HandArm->GetPickUpComponent()->SetOnlyOwnerSee(true);
			HandArm->GetPickUpComponent()->SetCastShadow(false);
			AA_Collectable* Collectable = Cast<AA_Collectable>(InNewHand->GetOwner());
			ensure(Collectable);
			HandArm->SetDummy(true, Collectable);
			HandArm->GetPickUpComponent()->OnObjectPickUp.Broadcast(this);
		}
	}
}

