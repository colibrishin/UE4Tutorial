// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyCV47Weapon.h"

#include "Kismet/GameplayStatics.h"

AMyCV47Weapon::AMyCV47Weapon()
{
	static ConstructorHelpers::FObjectFinder<USoundWave> SW_Fire(TEXT("SoundWave'/Game/Models/sounds/cv47-fire.cv47-fire'"));

	if (SW_Fire.Succeeded())
	{
		FireSound = SW_Fire.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> SW_Reload(TEXT("SoundWave'/Game/Models/sounds/cv47-reloading.cv47-reloading'"));

	if (SW_Reload.Succeeded())
	{
		ReloadSound = SW_Reload.Object;
	}
}

bool AMyCV47Weapon::AttackImpl()
{
	if (Super::AttackImpl())
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation(), GetActorRotation());
		return true;
	}

	return false;
}

bool AMyCV47Weapon::ReloadImpl()
{
	if (Super::ReloadImpl())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation(), GetActorRotation());
		return true;
	}

	return false;
}
