// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/Event/Weapons/ThrowWeapons/E_ExplosiveThrowWeapon.h"
#include "MyProject/Components/Weapon/C_ThrowWeapon.h"
#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Actors/BaseClass/A_ThrowWeapon.h"
#include "MyProject/MyPlayerState.h"
#include "MyProject/MyPlayerController.h"
#include "Engine/OverlapResult.h"
#include "Engine/DamageEvents.h"

void UE_ExplosiveThrowWeapon::DoEvent( TScriptInterface<IEventableContext> InContext )
{
	if ( UC_ThrowWeapon* WeaponComponent = Cast<UC_ThrowWeapon>( InContext.GetObject() ) )
	{
		float* Damage;
		float* Radius;
		GetParameter<float>( FText::FromString( "Damage" ) , &Damage );
		GetParameter<float>( FText::FromString( "Radius" ) , &Radius );

		if ( Damage && Radius )
		{
			TArray<FOverlapResult> HitResults;

			GetWorld()->OverlapMultiByChannel
			(
				OUT HitResults ,
				WeaponComponent->GetOwner()->GetActorLocation() ,
				FQuat::Identity ,
				ECC_Pawn ,
				FCollisionShape::MakeSphere( *Radius )
			);

			AA_Character* OriginCharacter = WeaponComponent->GetOrigin();
			AA_ThrowWeapon* Weapon = Cast<AA_ThrowWeapon>( WeaponComponent->GetOwner() );
			
			// Throw weapon component does not mutates the origin character of the spawned one.
			check( OriginCharacter );
			// Throw weapon component does not originates from the throw weapon.
			check( Weapon );

			for ( const auto& Result : HitResults )
			{
				if ( const AA_Character* Character = Cast<AA_Character>( Result.GetActor() ) )
				{
					const auto& Distance = FVector::Distance( Character->GetActorLocation() , Weapon->GetActorLocation() );
					const auto& Ratio = 1.f - Distance / *Radius;
					const auto& RatioDamage = (*Damage) * Ratio;

					if ( const auto& MyPlayerState = Character->GetPlayerState<AMyPlayerState>() )
					{
						// todo: team damage or self damage reduction
						MyPlayerState->TakeDamage
						(
							RatioDamage ,
							FDamageEvent{} ,
							OriginCharacter->GetController() ,
							OriginCharacter
						);
					}
				}
			}
		}
	}
}
