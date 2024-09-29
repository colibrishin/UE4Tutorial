


#include "A_Character.h"

#include "A_Collectable.h"
#include "EnhancedInputComponent.h"

#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Private/Utilities.hpp"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "MyProject/MyPlayerState.h"
#include "MyProject/Components/Asset/C_CharacterAsset.h"
#include "MyProject/Components/Asset/C_WeaponAsset.h"

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
	Hand = CreateDefaultSubobject<UChildActorComponent>(TEXT("HandActor"));
	ArmHand = CreateDefaultSubobject<UChildActorComponent>(TEXT("ArmHandActor"));

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
	
	Hand->SetNetAddressable();
	ArmHand->SetNetAddressable();
	Hand->SetIsReplicated(true);
	ArmHand->SetIsReplicated(true);
	
	AssetComponent->SetNetAddressable();
	AssetComponent->OnAssetIDSet.AddUObject(
		this, &AA_Character::FetchAsset);

	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCapsuleComponent()->InitCapsuleSize(88.f, 88.f);
	Hand->OnChildActorCreated().AddUObject(this, &AA_Character::SetupHand);
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

	if (bHandBusy)
	{
		LOG_FUNC_PRINTF( LogCharacter , Log , "Hand is busy, drop item %s back", *InPickUp->GetOwner()->GetName());
		InPickUp->OnObjectDrop.Broadcast(this, true);
		return;
	}

	const AA_Collectable* Collectable = Cast<AA_Collectable>(InPickUp->GetOwner());
	check(Collectable);
	ensureAlwaysMsgf(
		!Collectable->IsDummy(),
		TEXT("Undefined behaviour, PickUpComponet should be disabled if AA_Collectable is dummy"));

	// NOTE: CHILD ACTOR COMPONENT REPLICATION FLAG FOLLOWS THE REPLICATION FLAG OF THE CHILD ACTOR CDO
	// REPLICATION FLAG OF COMPONENT ITSELF DOES NOTHING IF ACTOR CLASS DOES NOT REPLICATES IN DEFAULT!!
	// see also ChildActorComponent.cpp:858
	Hand->SetChildActorClass(Collectable->GetClass());
	ArmHand->SetChildActorClass(Collectable->GetClass());
	
	// Hides the hand object;
	AActor* HandChild = Hand->GetChildActor();
	HandChild->SetOwner(this);
	HandChild->GetComponentByClass<UMeshComponent>()->SetOwnerNoSee(true);
	Cast<AA_Collectable>(HandChild)->SetDummy(false, nullptr);
	// Force the pick up event without pickup function calling to trigger the dependent components of
	// pick up component's event for setup such as weapon.
	HandChild->GetComponentByClass<UC_PickUp>()->OnObjectPickUp.Broadcast(this, false);

	// Replicates the arm hand child actor to owner only;
	AActor* ArmChild = ArmHand->GetChildActor();
	ArmChild->SetOwner(this);
	ArmChild->GetComponentByClass<UMeshComponent>()->SetOnlyOwnerSee(true);
	ArmChild->bOnlyRelevantToOwner = true;
	Cast<AA_Collectable>(ArmChild)->SetDummy(true, Cast<AA_Collectable>(Hand->GetChildActor()));
	ArmChild->GetComponentByClass<UC_PickUp>()->OnObjectPickUp.Broadcast(this, false);
	
	const int32 InPickUpID = Collectable->GetComponentByClass<UC_Asset>()->GetID();
	UC_Asset* CollectableAsset = Hand->GetChildActor()->GetComponentByClass<UC_Asset>();
	UC_Asset* ArmCollectableAsset = ArmHand->GetChildActor()->GetComponentByClass<UC_Asset>();
	
	CollectableAsset->SetID(InPickUpID);
	ArmCollectableAsset->SetID(InPickUpID);

	OnHandChanged.Broadcast(Hand);
	bHandBusy = true;
}

void AA_Character::Drop(UC_PickUp* InPickUp)
{
	IPickingUp::Drop(InPickUp);
	
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	const AA_Collectable* Collectable = Cast<AA_Collectable>(InPickUp->GetOwner());
	ensure(Collectable);
	
	// If Item is attached to character;
	if (const UC_PickUp* PickUp = Collectable->GetComponentByClass<UC_PickUp>();
		 PickUp && PickUp->GetAttachParentActor() == this)
	{
		UChildActorComponent** TargetComponent = Collectable->IsDummy() ? &ArmHand : &Hand;

		if (!Collectable->IsDummy())
		{
			bHandBusy = false;
			OnHandChanged.Broadcast(*TargetComponent);
			*TargetComponent = nullptr;
		}

		(*TargetComponent)->DestroyChildActor();
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
	DOREPLIFETIME_CONDITION(AA_Character, ArmHand, COND_OwnerOnly);
}

void AA_Character::OnRep_Hand() const
{
	OnHandChanged.Broadcast(Hand);
}

void AA_Character::SetupHand(AActor* InChildActor) const
{
	// OnChildActorCreated will be triggered only if child actor is valid;
	// todo: this is not working as intended.
	InChildActor->GetComponentByClass<UMeshComponent>()->SetCastShadow(false);
}

void AA_Character::PostFetchAsset()
{
	IAssetFetchable::PostFetchAsset();

	Hand->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocketName);
	ArmHand->AttachToComponent(ArmMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocketName);
}

