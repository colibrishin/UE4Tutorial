// Fill out your copyright notice in the Description page of Project Settings.


#include "SS_World.h"
USS_World::USS_World()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Asset(TEXT("/Script/Engine.DataTable'/Game/Data/DataAssets.DataAssets'"));

	if (DT_Asset.Succeeded())
	{
		AssetTable = DT_Asset.Object;
	}
}
