


#include "A_Character.h"

#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Private/Utilities.hpp"
#include "EnhancedInputSubsystems.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"

#include "MyProject/MyPlayerState.h"
#include "MyProject/Components/Asset/C_CharacterAsset.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogCharacter);

const FName AA_Character::LeftHandSocketName( TEXT( "hand_l_socket" ) );
const FName AA_Character::RightHandSocketName( TEXT( "hand_r_socket" ) );
const FName AA_Character::HeadSocketName( TEXT( "head_socket" ) );
const FName AA_Character::ChestSocketName( TEXT( "Chest" ) );

// Sets default values
AA_Character::AA_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ArmMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
	AssetComponent = CreateDefaultSubobject<UC_CharacterAsset>(TEXT("AssetComponent"));
	Camera1P = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	ArmMeshComponent->SetupAttachment(GetCapsuleComponent());
	Camera1P->SetupAttachment(GetCapsuleComponent());

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetCastShadow(true);
	ArmMeshComponent->SetOnlyOwnerSee(true);
	ArmMeshComponent->SetCastShadow(false);
	
	AssetComponent->SetNetAddressable();

	bReplicates = true;
	ACharacter::SetReplicateMovement(true);
	bNetLoadOnClient = true;
}

// Called when the game starts or when spawned
void AA_Character::BeginPlay()
{
	Super::BeginPlay();

	if ( const APlayerController* PlayerController = Cast<APlayerController>( Controller ) )
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

	if (bHandBusy)
	{
		LOG_FUNC_PRINTF( LogCharacter , Log , "Hand is busy, drop item %s back", *InPickUp->GetOwner()->GetName());
		InPickUp->OnObjectDrop.Broadcast( this );
		return;
	}

	if (InPickUp->GetOwner()->GetComponentByClass<USkeletalMeshComponent>()->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		RightHandSocketName
	) )
	{
		bHandBusy = true;
		OnHandChanged.Broadcast(nullptr, InPickUp);
		Hand = InPickUp;
	}
	else
	{
		ensureAlwaysMsgf( false, TEXT("%s; Unable to attach item %s to the hand;") , *GetName() , *InPickUp->GetOwner()->GetName());
	}
}

void AA_Character::Drop(UC_PickUp* InPickUp)
{
	IPickingUp::Drop(InPickUp);

	// If Item is attached to character;
	if (UC_PickUp* PickUp = InPickUp->GetOwner()->GetComponentByClass<UC_PickUp>();
		 PickUp && PickUp->GetAttachParentActor() == this)
	{
		PickUp->DetachFromComponent( FDetachmentTransformRules::KeepWorldTransform );
		bHandBusy = false;
		OnHandChanged.Broadcast(PickUp, nullptr);
		Hand = nullptr;
	}
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
}

void AA_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AA_Character, bHandBusy);
	DOREPLIFETIME(AA_Character, Hand);
	DOREPLIFETIME(AA_Character, AssetComponent);
}

void AA_Character::OnRep_Hand(UC_PickUp* InOldHand) const
{
	OnHandChanged.Broadcast(InOldHand, Hand);
}

