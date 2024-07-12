// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyCabinWeapon.h"

AMyCabinWeapon::AMyCabinWeapon()
{
	static ConstructorHelpers::FObjectFinder<USoundWave> SW_Fire(TEXT("SoundWave'/Game/Models/sounds/m4a1-fire.m4a1-fire'"));

	if (SW_Fire.Succeeded())
	{
		FireSound = SW_Fire.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> SW_Reload(TEXT("SoundWave'/Game/Models/sounds/m4a1-reload.m4a1-reload'"));

	if (SW_Reload.Succeeded())
	{
		ReloadSound = SW_Reload.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Model(TEXT("SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/AR4/SK_AR4_X.SK_AR4_X'"));

	if (SK_Model.Succeeded())
	{
		GetSkeletalMeshComponent()->SetSkeletalMesh(SK_Model.Object);
	}
}
