// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ThrowWeapon.h"

#include "Camera/CameraComponent.h"
#include "Components/ShapeComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Actors/BaseClass/A_ThrowWeapon.h"
#include "MyProject/Interfaces/PickingUp.h"
#include "MyProject/Components/Asset/C_WeaponAsset.h"
#include "MyProject/DataAsset/DA_ThrowWeapon.h"
#include "MyProject/Frameworks/Subsystems/SS_EventGameInstance.h"

class AA_ThrowWeapon;
// Sets default values for this component's properties
UC_ThrowWeapon::UC_ThrowWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}

// Called when the game starts
void UC_ThrowWeapon::BeginPlay()
{
	Super::BeginPlay();

	// ...

	OnStopAttack.AddUniqueDynamic(this, &UC_ThrowWeapon::Throw);

	// use the OnObjectDropPreSpawned instead of OnStopAttack;
	// new object will be created while in drop, throwing force and setup need to be applied to the new object
	// note that at OnObjectDropPreSpawned point, this object is not destroyed yet;
	UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>();
	// ensure that pickup component is available;
	check(PickUpComponent);
	PickUpComponent->OnObjectDropPreSpawned.AddUniqueDynamic(this, &UC_ThrowWeapon::SetUpSpawnedObject);
	
}

void UC_ThrowWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	if (bFiring)
	{
		CookTimeCounter += DeltaTime;
	}
}

void UC_ThrowWeapon::SetUpSpawnedObject(AActor* InSpawnedActor)
{
	const UDA_ThrowWeapon* WeaponAsset = Cast<UDA_ThrowWeapon>( ReferenceAsset );
	check( WeaponAsset );

	const AActor* WeaponOwner = GetOwner()->GetAttachParentActor();
	const float CookTimeRatio = CookTimeCounter / WeaponAsset->GetCookingTime();
	const FVector ForwardVector = GetOwner()->GetOwner()->GetActorForwardVector();
	const float Force = WeaponAsset->GetThrowForce() * (CookTimeRatio * WeaponAsset->GetThrowMultiplier());
	
	// Spawned actor is not throw weapon
	AA_ThrowWeapon* ThrowWeapon = Cast<AA_ThrowWeapon>( InSpawnedActor );
	check( ThrowWeapon );

	ThrowWeapon->bAlwaysRelevant = true;
	ThrowWeapon->GetCollisionComponent()->SetSimulatePhysics( false );
	ThrowWeapon->GetCollisionComponent()->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );

	const FRotator Rotator( 45.f , 0.f , 0.f );
	ThrowWeapon->GetProjectileMovementComponent()->SetUpdatedComponent( ThrowWeapon->GetCollisionComponent() );
	ThrowWeapon->GetProjectileMovementComponent()->SetActive( true );
	ThrowWeapon->GetProjectileMovementComponent()->InitialSpeed = Force;
	ThrowWeapon->GetProjectileMovementComponent()->Velocity = Rotator.RotateVector( ForwardVector ).GetSafeNormal() * Force;

	UC_ThrowWeapon* SpawnedComponent = Cast<UC_ThrowWeapon>( ThrowWeapon->GetWeaponComponent() );
	AA_Character* Origin = Cast<AA_Character>( GetOwner()->GetParentActor() );
	// Throw weapon originates from unknown actor
	check( Origin );
	// Throw weapon component not found
	check( SpawnedComponent );

	// Mutates the origin character of the spawned throw weapon.
	SpawnedComponent->SetOrigin( Origin );

	// Do not allow to pick up the throw weapon again.
	// todo: pick and throw back?
	ThrowWeapon->GetPickUpComponent()->AttachEventHandlers( false , EPickUp::Drop );

	FTimerDelegate                                                  Delegate;
	Delegate.BindLambda( []( const TScriptInterface<IEventHandler>& Handler, const TScriptInterface<IEventableContext>& InContext, const UDA_ThrowWeapon* InAsset)
	{
		Handler->DoEvent( InContext , InAsset->GetParameters() );
	}, EventHandler, this, WeaponAsset);

	GetWorld()->GetTimerManager().SetTimer(
		SpawnedComponent->ThrowAfterEventTimer ,
		Delegate ,
		WeaponAsset->GetEventTimeAfterThrow(),
		false ,
		-1
	);

	CookTimeCounter = 0.f;
}

void UC_ThrowWeapon::Throw(UC_Weapon* InWeapon)
{
	// use the drop weapon sequence;
	AA_Character*    OwningActor     = Cast<AA_Character>(GetOwner()->GetAttachParentActor());
	const UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>();
	check(PickUpComponent);
	PickUpComponent->OnObjectDrop.Broadcast(OwningActor, true);
}

void UC_ThrowWeapon::UpdateFrom( UDA_Weapon* InAsset )
{
	Super::UpdateFrom( InAsset );

	if (const UDA_ThrowWeapon* WeaponAsset = Cast<UDA_ThrowWeapon>(InAsset))
	{
		if ( const TSubclassOf<UObject> Handler = WeaponAsset->GetEventHandler();
			 Handler && GetNetMode() != NM_Client )
		{
			EventHandler = GetWorld()->GetGameInstance()->GetSubsystem<USS_EventGameInstance>()->AddEvent( Handler );
		}
	}
}

void UC_ThrowWeapon::HandlePickUp( TScriptInterface<IPickingUp> InPickUpObject , const bool bCallPickUp )
{
	Super::HandlePickUp( InPickUpObject , bCallPickUp );

	if ( GetNetMode() == NM_Client )
	{
		return;
	}

	const AA_ThrowWeapon* ThrowWeapon = Cast<AA_ThrowWeapon>( GetOwner() );
	check( ThrowWeapon );

	ThrowWeapon->GetProjectileMovementComponent()->SetActive( false );
}

void UC_ThrowWeapon::SetOrigin( AA_Character* InCharacter )
{
	OriginCharacter = InCharacter;
}

AA_Character* UC_ThrowWeapon::GetOrigin() const
{
	return OriginCharacter;
}
