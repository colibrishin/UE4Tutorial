


#include "A_Character.h"

#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Private/Utilities.hpp"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

DEFINE_LOG_CATEGORY( LogCharacter );

const FName AA_Character::LeftHandSocketName( TEXT( "hand_l_socket" ) );
const FName AA_Character::RightHandSocketName( TEXT( "hand_r_socket" ) );
const FName AA_Character::HeadSocketName( TEXT( "head_socket" ) );
const FName AA_Character::ChestSocketName( TEXT( "Chest" ) );

// Sets default values
AA_Character::AA_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
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
	IPickableObject::PickUp(InPickUp);

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
	}
	else
	{
		ensureAlwaysMsgf( false, TEXT("%s; Unable to attach item %s to the hand;") , *GetName() , *InPickUp->GetOwner()->GetName());
	}
}

void AA_Character::Drop(UC_PickUp* InPickUp)
{
	IPickableObject::Drop(InPickUp);

	// If Item is attached to character;
	if (USkeletalMeshComponent* ObjectMesh = InPickUp->GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
		 ObjectMesh && ObjectMesh->GetAttachParentActor() == this)
	{
		ObjectMesh->DetachFromComponent( FDetachmentTransformRules::KeepWorldTransform );
		bHandBusy = false;
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

