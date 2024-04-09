// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyCameraManager.h"

AMyCameraManager::AMyCameraManager()
{
	DefaultFOV = 90.0f;

	ViewPitchMin = -90.0f;
	ViewPitchMax = 90.0f;

	ViewYawMin = 0;
	ViewYawMax = 359.99f;

	ViewRollMin = -90.0f;
	ViewRollMax = 90.0f;
}
