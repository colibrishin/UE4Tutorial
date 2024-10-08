


#include "A_Character.h"

#include "A_Collectable.h"
#include "EnhancedInputComponent.h"

#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Private/Utilities.hpp"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"

#include "Engine/OverlapResult.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"

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

	if (static ConstructorHelpers::FObjectFinder<UInputAction> IA_PickUp(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Inputs/IA_PickUp.IA_PickUp'"));
		 IA_PickUp.Succeeded())
	{
		PickUpAction = IA_PickUp.Object;
	}

	if (static ConstructorHelpers::FObjectFinder<UInputAction> IA_Drop(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Inputs/IA_Drop.IA_Drop'"));
		 IA_Drop.Succeeded())
	{
		DropAction = IA_Drop.Object;
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
}

// Called when the game starts or when spawned
void AA_Character::BeginPlay()
{
	Super::BeginPlay();

	if ( const APlayerController* PlayerController = Cast<APlayerController>( Controller );
		 PlayerController && PlayerController->IsLocalPlayerController())
	{
		if ( UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( PlayerController->GetLocalPlayer() ) )
		{
			Subsystem->AddMappingContext( InputMapping , 0 );
		}
	}

	if (GetNetMode() == NM_Client)
	{
		OnHandChanged.AddUFunction(this, "SyncHandProperties");
	}
}

void AA_Character::PickUp(UC_PickUp* InPickUp)
{
	IPickingUp::PickUp(InPickUp);

	if (GetNetMode() == NM_Client)
	{
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
	
	AA_Collectable* HandChild = Cast<AA_Collectable>(Hand->GetChildActor());
	AA_Collectable* ArmChild = Cast<AA_Collectable>(ArmHand->GetChildActor());

	HandChild->SetOwner(this);
	ArmChild->SetOwner(this);

	const int32 InPickUpID = Collectable->GetComponentByClass<UC_Asset>()->GetID();
	UC_Asset* CollectableAsset = Hand->GetChildActor()->GetComponentByClass<UC_Asset>();
	UC_Asset* ArmCollectableAsset = ArmHand->GetChildActor()->GetComponentByClass<UC_Asset>();
	
	CollectableAsset->SetID(InPickUpID);
	ArmCollectableAsset->SetID(InPickUpID);

	HandChild->SetPhysics(false);
	ArmChild->SetPhysics(false);

	Cast<AA_Collectable>(HandChild)->SetDummy(false, nullptr);
	Cast<AA_Collectable>(ArmChild)->SetDummy(true, Cast<AA_Collectable>(Hand->GetChildActor()));

	// Replicates the hand child actor to everyone;
	HandChild->bAlwaysRelevant = true;
	// Replicates the arm hand child actor to owner only;
	ArmChild->bOnlyRelevantToOwner = true;
	
	// Broadcast the pick up component to trigger any pick up event dependent listeners;
	HandChild->GetComponentByClass<UC_PickUp>()->OnObjectPickUp.Broadcast(this, false);
	ArmChild->GetComponentByClass<UC_PickUp>()->OnObjectPickUp.Broadcast(this, false);
	
	HandActor = Cast<AA_Collectable>(HandChild);
	ArmHandActor = Cast<AA_Collectable>(ArmChild);
	SyncHandProperties();

	OnHandChanged.Broadcast(HandActor);
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
	ensureAlwaysMsgf(
		!Collectable->IsDummy(),
		TEXT("Undefined behaviour, PickUpComponet should be disabled if AA_Collectable is dummy"));
	
	// If Item is attached to character;
	if (const UC_PickUp* PickUp = Collectable->GetComponentByClass<UC_PickUp>();
		 PickUp && PickUp->GetOwner()->GetAttachParentActor() == this)
	{
		bHandBusy = false;
		OnHandChanged.Broadcast(nullptr);
		Hand->DestroyChildActor();
		ArmHand->DestroyChildActor();
		HandActor = nullptr;
		ArmHandActor = nullptr;
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

		EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &AA_Character::Server_PickUp);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &AA_Character::Server_Drop);
	}
}

void AA_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AA_Character, bHandBusy);
	DOREPLIFETIME(AA_Character, AssetComponent);
	
	DOREPLIFETIME(AA_Character, Hand);
	DOREPLIFETIME_CONDITION(AA_Character, ArmHand, COND_OwnerOnly);
	DOREPLIFETIME(AA_Character, HandActor);
	DOREPLIFETIME_CONDITION(AA_Character, ArmHandActor, COND_OwnerOnly);
}

void AA_Character::OnRep_Hand() const
{
	OnHandChanged.Broadcast(HandActor);
}

void AA_Character::SyncHandProperties() const
{
	const auto& SyncProperties = [this](AA_Collectable* InChild)
	{
		// Since resetting Hand with nullptr exists, validity should be checked; 
		if (!IsValid(InChild))
		{
			LOG_FUNC(LogCharacter, Log, "Empty hand, ignore property sync");
			return;
		}
		
		LOG_FUNC_PRINTF(LogCharacter, Log, "Set default hand properties to %s", *InChild->GetName());
		if (UMeshComponent* MeshComponent = InChild->GetComponentByClass<UMeshComponent>())
		{
			MeshComponent->SetCastShadow(false);
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	};

	SyncProperties(HandActor);

	// Arm hand is replicated to only owner;
	if (ArmHandActor)
	{
		SyncProperties(ArmHandActor);
	}
}

void AA_Character::PostFetchAsset()
{
	IAssetFetchable::PostFetchAsset();

	Hand->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocketName);
	ArmHand->AttachToComponent(ArmMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocketName);
}

void AA_Character::Server_PickUp_Implementation()
{
	TArray<FOverlapResult> OutResult;
	FVector Center, Extents;
	GetActorBounds(true, Center, Extents);

	// Ignore the character just in case;
	FCollisionQueryParams Params{NAME_None, false, this};
	
	// Ignore hand collectable objects;
	if (HandActor)
	{
		Params.AddIgnoredActor(HandActor);
		Params.AddIgnoredActor(ArmHandActor);
	}
	
	if (!GetWorld()->OverlapMultiByChannel
		(
		 OutResult ,
		 GetActorLocation() ,
		 FQuat::Identity ,
		 ECC_GameTraceChannel9 ,
		 FCollisionShape::MakeSphere(Extents.GetMax()) ,
		 Params
		))
	{
		return;
	}

	// Sort the objects in distance wise;
	OutResult.Sort([this](const FOverlapResult& InLeft, const FOverlapResult& InRight)
	{
		const float& LeftDistance = FVector::Distance(GetActorLocation(), InLeft.GetActor()->GetActorLocation());
		const float& RightDistance = FVector::Distance(GetActorLocation(), InRight.GetActor()->GetActorLocation());
		return LeftDistance < RightDistance; 
	});

	const AA_Collectable* Collectable = Cast<AA_Collectable>((*OutResult.begin()).GetActor());
	check(Collectable);

	// todo: inventory;
	if (bHandBusy)
	{
		LOG_FUNC_PRINTF( LogCharacter , Log , "Hand is busy, ignore pick up request", *Collectable->GetOwner()->GetName());
		return;
	}

	Collectable->GetPickUpComponent()->OnObjectPickUp.Broadcast(this, true);
}

void AA_Character::Server_Drop_Implementation()
{
	if (HandActor)
	{
		HandActor->GetPickUpComponent()->OnObjectDrop.Broadcast(this, true);
	}
}

