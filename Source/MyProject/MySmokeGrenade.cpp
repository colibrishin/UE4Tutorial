// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MySmokeGrenade.h"

#include "NiagaraComponent.h"

AMySmokeGrenade::AMySmokeGrenade()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Grenade(TEXT("SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/G67_Grenade/SK_G67_X.SK_G67_X'"));

	SetSkeletalMesh();

	if (SK_Grenade.Succeeded())
	{
		GetSkeletalMeshComponent()->SetSkeletalMesh(SK_Grenade.Object);
	}

	SmokeEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeEffect"));
	SmokeEffect->SetupAttachment(GetRootComponent());
	
	SmokeEffect->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_SmokeEffect(TEXT("NiagaraSystem'/Game/Blueprints/BPNSSmoke.BPNSSmoke'"));

	if (NS_SmokeEffect.Succeeded())
	{
		SmokeEffect->SetAsset(NS_SmokeEffect.Object);
	}
}

void AMySmokeGrenade::OnExplosionTimerExpiredImpl()
{
	Super::OnExplosionTimerExpiredImpl();

	SmokeEffect->Activate();

	GetWorldTimerManager().SetTimer
	(
		IN OnSmokeEffectExpiredHandle,
		this,
		&AMySmokeGrenade::OnSmokeEffectExpired,
		5.f,
		false
	);
}

void AMySmokeGrenade::OnSmokeEffectExpired()
{
	SmokeEffect->Deactivate();
	Destroy(true);
}
