// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "MyStatComponent.generated.h"


class AMyCharacter;
class UMyAnimInstance;
struct FMyStat;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyStatComponent();

	FORCEINLINE int32 GetLevel() const { return Level; }
	FORCEINLINE int32 GetDamage() const { return Damage; }
	FORCEINLINE int32 GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE USkeletalMesh* GetSkeletalMesh() const { return SkeletalMesh; }
	FORCEINLINE USkeletalMesh* GetArmSkeletalMesh() const { return ArmSkeletalMesh; }
	FORCEINLINE TSubclassOf<UAnimInstance> GetAnimInstance() const { return AnimInstance; }
	FORCEINLINE TSubclassOf<UAnimInstance> GetArmAnimInstance() const { return ArmAnimInstance; }
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Stats", Meta=(AllowPrivateAccess))
	int32 Level;

	UPROPERTY(VisibleAnywhere, Category = "Stats", Meta=(AllowPrivateAccess))
	int32 Damage;

	UPROPERTY(VisibleAnywhere, Category = "Stats", Meta=(AllowPrivateAccess))
	int32 MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "Meshes", meta=(AllowPrivateAccess))
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, Category = "Meshes", meta=(AllowPrivateAccess))
	USkeletalMesh* ArmSkeletalMesh;

	UPROPERTY(VisibleAnywhere, Category = "Animations", meta=(AllowPrivateAccess))
	TSubclassOf<UAnimInstance> AnimInstance;
	
	UPROPERTY(VisibleAnywhere, Category = "Animations", meta=(AllowPrivateAccess))
	TSubclassOf<UAnimInstance> ArmAnimInstance;
	
};
