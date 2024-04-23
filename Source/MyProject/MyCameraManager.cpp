// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyCameraManager.h"

AMyCameraManager::AMyCameraManager()
{
	DefaultFOV = 90.0f;

	ViewPitchMin = -89.90f;
	ViewPitchMax = 89.90f;

	ViewYawMin = 0;
	ViewYawMax = 359.99f;
}
