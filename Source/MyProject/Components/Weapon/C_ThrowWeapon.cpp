// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ThrowWeapon.h"

#include "Camera/CameraComponent.h"
#include "Components/ShapeComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Actors/BaseClass/A_ThrowWeapon.h"
#include "MyProject/Interfaces/PickingUp.h"


class AA_ThrowWeapon;
// Sets default values for this component's properties
UC_ThrowWeapon::UC_ThrowWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bCanSpray = false;
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
	const AActor* WeaponOwner = GetOwner()->GetAttachParentActor();
	const float CookTimeRatio = CookTimeCounter / CookingTime;
	const FVector ForwardVector = GetOwner()->GetOwner()->GetActorForwardVector();
	const float Force = ThrowForce * (CookTimeRatio * ThrowForceMultiplier);

	if (AA_ThrowWeapon* ThrowWeapon = Cast<AA_ThrowWeapon>(InSpawnedActor))
	{
		FRotator Rotator( 45.f , 0.f , 0.f );
		ThrowWeapon->GetProjectileMovementComponent()->bSimulationEnabled = true;
		ThrowWeapon->GetProjectileMovementComponent()->InitialSpeed = Force;
		ThrowWeapon->GetProjectileMovementComponent()->Velocity = Rotator.RotateVector(ForwardVector * Force);
	}

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

void UC_ThrowWeapon::HandlePickUp( TScriptInterface<IPickingUp> InPickUpObject , const bool bCallPickUp )
{
	Super::HandlePickUp( InPickUpObject , bCallPickUp );

	if ( GetNetMode() == NM_Client )
	{
		return;
	}

	AA_ThrowWeapon* ThrowWeapon = Cast<AA_ThrowWeapon>( GetOwner() );
	check( ThrowWeapon );

	ThrowWeapon->GetProjectileMovementComponent()->bSimulationEnabled = false;
}
