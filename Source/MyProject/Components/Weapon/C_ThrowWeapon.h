// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_Weapon.h"
#include "MyProject/Interfaces/EventableContext.h"
#include "MyProject/Interfaces/EventHandler.h"
#include "C_ThrowWeapon.generated.h"


UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_ThrowWeapon : public UC_Weapon, public IEventableContext
{
	GENERATED_BODY()

public:
	friend class UC_WeaponAsset;
	
	// Sets default values for this component's properties
	UC_ThrowWeapon();

	AA_Character* GetOrigin() const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void SetUpSpawnedObject(AActor* InSpawnedActor);

	UFUNCTION()
	void Throw(UC_Weapon* InWeapon);

	void HandlePickUp( TScriptInterface<IPickingUp> InPickUpObject , const bool bCallPickUp ) override;

	void SetOrigin( AA_Character* InCharacter );

private:
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	float CookTimeCounter;

	UPROPERTY(VisibleAnywhere, Category="Throwable", meta=(AllowPrivateAccess))
	float CookingTime;
	
	UPROPERTY(VisibleAnywhere, Category="Throwable", meta=(AllowPrivateAccess))
	float ThrowForce;

	UPROPERTY(VisibleAnywhere, Category="Throwable", meta=(AllowPrivateAccess))
	float ThrowForceMultiplier;

	UPROPERTY( VisibleAnywhere , Category = "Throwable" , meta = ( AllowPrivateAccess ) )
	float EventTimeAfterThrow;

	UPROPERTY(VisibleAnywhere, Category="Origin", meta=(AllowPrivateAccess))
	AA_Character* OriginCharacter;

	TScriptInterface<IEventHandler> EventHandler;

	FTimerHandle ThrowAfterEventTimer;

};
