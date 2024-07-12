// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyCV47Weapon.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

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

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Model(TEXT("SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/Ka47/SK_KA47_X.SK_KA47_X'"));

	if (SK_Model.Succeeded())
	{
		GetSkeletalMeshComponent()->SetSkeletalMesh(SK_Model.Object);
	}
}
