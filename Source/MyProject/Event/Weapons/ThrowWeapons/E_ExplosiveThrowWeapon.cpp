// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/Event/Weapons/ThrowWeapons/E_ExplosiveThrowWeapon.h"
#include "MyProject/Components/Weapon/C_ThrowWeapon.h"
#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Actors/BaseClass/A_ThrowWeapon.h"
#include "MyProject/MyPlayerState.h"
#include "MyProject/MyPlayerController.h"
#include "MyProject/Private/Utilities.hpp"
#include "Engine/OverlapResult.h"
#include "Engine/DamageEvents.h"

void UE_ExplosiveThrowWeapon::DoEvent( TScriptInterface<IEventableContext> InContext, const FParameters& InParameters ) const
{
	if ( const UC_ThrowWeapon* WeaponComponent = Cast<UC_ThrowWeapon>( InContext.GetObject() ) )
	{
		LOG_FUNC( LogTemp , Log , "Explosion event triggered" );
		const float Damage = WeaponComponent->GetDamage();
		const float Radius = InParameters.FloatParameters["Radius"];

		TArray<FOverlapResult> HitResults;

		FVector WeaponLocation = WeaponComponent->GetOwner()->GetActorLocation();
		// todo: translate the explosion force to the near objects.
		WeaponComponent->GetWorld()->OverlapMultiByChannel
		(
			OUT HitResults ,
			WeaponLocation ,
			FQuat::Identity ,
			ECC_Pawn ,
			FCollisionShape::MakeSphere( Radius )
		);

		DrawDebugSphere( WeaponComponent->GetWorld() , WeaponLocation , 500.f , 15 , ( HitResults.IsEmpty() ? FColor::Red : FColor::Green ) , false , 2.f );

		AA_Character*         OriginCharacter = WeaponComponent->GetOrigin();
		AA_ThrowWeapon* Weapon          = Cast<AA_ThrowWeapon>( WeaponComponent->GetOwner() );
		
		// Throw weapon component does not mutates the origin character of the spawned one.
		check( OriginCharacter );
		// Throw weapon component does not originates from the throw weapon.
		check( Weapon );

		for ( const auto& Result : HitResults )
		{
			if ( AA_Character* Character = Cast<AA_Character>( Result.GetActor() ) )
			{
				LOG_FUNC_PRINTF( LogTemp , Log , "Found Character: %s" , *Character->GetName() );

				const auto& Distance = FVector::Distance( Character->GetActorLocation() , Weapon->GetActorLocation() );
				const auto& Ratio = 1.f - Distance / Radius;
				const auto& RatioDamage = Damage * Ratio;

				// todo: team damage or self damage reduction
				Character->TakeDamage
				(
					RatioDamage ,
					FDamageEvent{} ,
					OriginCharacter->GetController() ,
					OriginCharacter
				);
			}
		}

		Weapon->Destroy();
	}
}
