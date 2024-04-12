// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyMeleeWeapon.h"

bool AMyMeleeWeapon::AttackImpl()
{
	return true;
}

bool AMyMeleeWeapon::ReloadImpl()
{
	return true;
}

void AMyMeleeWeapon::OnFireRateTimed()
{
	Super::OnFireRateTimed();
}

void AMyMeleeWeapon::OnReloadDone()
{
	Super::OnReloadDone();
}
