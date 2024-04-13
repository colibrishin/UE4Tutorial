// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "MyAnimInstance.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "DrawDebugHelpers.h"
#include "MyStatComponent.h"
#include "MyWeapon.h"
#include "ConstantFVector.hpp"
#include "MyAIController.h"
#include "MyAimableWeapon.h"
#include "MyC4.h"
#include "MyCharacterWidget.h"
#include "MyInGameHUD.h"
#include "MyInventoryComponent.h"

#include "Components/WidgetComponent.h"

#include "Net/UnrealNetwork.h"

const FName AMyCharacter::LeftHandSocketName(TEXT("hand_l_socket"));
const FName AMyCharacter::RightHandSocketName(TEXT("hand_r_socket"));
const FName AMyCharacter::HeadSocketName(TEXT("head_socket"));
const FName AMyCharacter::ChestSocketName(TEXT("Chest"));

// Sets default values
AMyCharacter::AMyCharacter() : CanAttack(true)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 리소스를 불러오는 방법
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mesh(TEXT("SkeletalMesh'/Game/ParagonBoris/Characters/Heroes/Boris/Meshes/Boris.Boris'"));

	if (SK_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Mesh.Object);
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	StatComponent = CreateDefaultSubobject<UMyStatComponent>(TEXT("StatComponent"));
	Inventory = CreateDefaultSubobject<UMyInventoryComponent>(TEXT("Inventory"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	// 기본 캡슐 사이즈, 매쉬가 붕 뜨지 않게 하도록
	// Character 구현부 52번 라인 참조
	// 캐릭터의 바닥을 맞추기 위해 사용할 수도 있음
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	SpringArm->TargetArmLength = 0.f;
	SpringArm->SetRelativeLocation({93.f, 41.f, 84.f});

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;

	AttackIndex = 0;

	Weapon = nullptr;

	AIControllerClass = AMyAIController::StaticClass();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->SetOwnerNoSee(true);

}

void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	const auto& Anim = GetMesh()->GetAnimInstance();

	if (IsValid(Anim))
	{
		AnimInstance = Cast<UMyAnimInstance>(Anim);
		AnimInstance->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnMontageEnded);
		AnimInstance->BindOnAttackHit(this, &AMyCharacter::OnAttackAnimNotify);
	}
}

float AMyCharacter::TakeDamage(
	float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser
)
{
	StatComponent->OnDamage(Damage);
	UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), Damage);
	return Damage;
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCharacter, Weapon);
	DOREPLIFETIME(AMyCharacter, CurrentItem);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMyCharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("Interactive"), IE_Pressed, this, &AMyCharacter::Interactive);
	PlayerInputComponent->BindAction(TEXT("Interactive"), IE_Repeat, this, &AMyCharacter::Interactive);
	PlayerInputComponent->BindAction(TEXT("Interactive"), IE_Released, this, &AMyCharacter::InteractInterrupted);

	PlayerInputComponent->BindAction(TEXT("Use"), IE_Repeat, this, &AMyCharacter::Use);
	PlayerInputComponent->BindAction(TEXT("Use"), IE_Released, this, &AMyCharacter::UseInterrupt);

	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Repeat, this, &AMyCharacter::Aim);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &AMyCharacter::UnAim);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AMyCharacter::Reload);

	// Somehow BindAction with IE_Repeat doesn't work, move Attack to axis.
	PlayerInputComponent->BindAxis(TEXT("Attack"), this, &AMyCharacter::Attack);
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("Pitch"), this, &AMyCharacter::Pitch);
	PlayerInputComponent->BindAxis(TEXT("Yaw"), this, &AMyCharacter::Yaw);

}

bool AMyCharacter::TryPickWeapon(AMyWeapon* NewWeapon)
{
	if (IsValid(Weapon))
	{
		return false;
	}

	if (IsValid(NewWeapon))
	{
		Weapon = NewWeapon;

		if (Weapon->IsA<AMyAimableWeapon>())
		{
			const auto& AimableWeapon = Cast<AMyAimableWeapon>(Weapon);

			if (!IsValid(AimableWeapon))
			{
				return false;
			}

			UE_LOG(LogTemp, Warning, TEXT("AimableWeapon: %s"), *AimableWeapon->GetName());
			AimableWeapon->Show();
			AimableWeapon->AttachToComponent(
				GetMesh(), 
				FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
				RightHandSocketName);

			return true;
		}

		UE_LOG(LogTemp, Warning, TEXT("Weapon: %s"), *Weapon->GetName());
		Weapon->Show();
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandSocketName);
		return true;
	}

	return false;
}

void AMyCharacter::Server_Attack_Implementation(const float Value)
{
	if (!CanAttack)
	{
		return;
	}

	if (IsValid(Weapon) && !Weapon->CanDoAttack())
	{
		return;
	}

	Multi_Attack(Value);
}


void AMyCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AnimInstance->GetAttackMontage())
	{
		ResetAttack();
	}
}

void AMyCharacter::Reload()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	if (!HasAuthority())
	{
		Server_Reload();
	}
	else if (HasAuthority() || IsRunningDedicatedServer())
	{
		Multi_Reload();
	}
}

void AMyCharacter::Server_Reload_Implementation()
{
	if (!IsValid(Weapon))
	{
		return;
	}

	if (!Weapon->CanBeReloaded())
	{
		return;
	}

	Multi_Reload();
}

void AMyCharacter::HitscanAttack()
{
	if (!CanAttack)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Hitscan Fire!"));

	FHitResult HitResult;
	FCollisionQueryParams Params{NAME_None, false, this};

	FVector EndVector = 
		Camera->GetComponentLocation() + 
		Camera->GetForwardVector() * 
		Weapon->GetWeaponStatComponent()->GetRange();

	const auto& Result = GetWorld()->LineTraceSingleByChannel
		(
		 OUT HitResult,
		 Camera->GetComponentLocation() ,
		 EndVector,
		 ECollisionChannel::ECC_Visibility,
		 Params
		);

	if (Result)
	{
		DrawDebugLine
			(
			 GetWorld(),
			 Camera->GetComponentLocation(),
			 HitResult.ImpactPoint,
			 FColor::Green,
			 false,
			 1.f,
			 0,
			 2.f
			);

		const auto& Target = Cast<AMyCharacter>(HitResult.GetActor());

		if (IsValid(Target))
		{
			UE_LOG(LogTemp, Warning , TEXT("Hit Actor: %s"), *Target->GetName());
			FDamageEvent DamageEvent;
			Target->TakeDamage(GetDamage(), DamageEvent, GetController(), this);
		}
	}
	else
	{
		DrawDebugLine
			(
			 GetWorld(),
			 Camera->GetComponentLocation(),
			 EndVector,
			 FColor::Red,
			 false,
			 1.f,
			 0,
			 2.f
			);
	}
}

void AMyCharacter::MeleeAttack()
{
	if (!CanAttack)
	{
		return;
	}

	constexpr int32 MaxAttackSection = 3;

	UE_LOG(LogTemp, Warning, TEXT("Melee Attack"));
	AttackIndex = (AttackIndex + 1) % MaxAttackSection;
	AnimInstance->PlayAttackMontage(AttackIndex);
}

void AMyCharacter::Multi_Reload_Implementation()
{
	ReloadStart();
}

void AMyCharacter::ReloadStart() const
{
	if (!IsValid(Weapon))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Reload"));
	Weapon->Reload();
}

void AMyCharacter::UpDown(const float Value)
{
	// == Vector3{1, 0, 0} * acceleration
	GetCharacterMovement()->AddInputVector(GetActorForwardVector() * Value);
	ForwardInput = Value;
}
void AMyCharacter::LeftRight(const float Value)
{
	// == Vector3{1, 0, 0} * acceleration
	GetCharacterMovement()->AddInputVector(GetActorRightVector() * Value);
	RightInput = Value;
}

void AMyCharacter::Aim()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	if (!IsValid(Weapon))
	{
		return;
	}

	if (!Weapon->IsA<AMyAimableWeapon>())
	{
		return;
	}

	if (IsAiming)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Aim"));

	IsAiming = true;
	OnAiming.Broadcast(IsAiming);
}

void AMyCharacter::UnAim()
{
	if (!IsAiming)
	{
		return;
	}

	IsAiming = false;
	OnAiming.Broadcast(IsAiming);
}

void AMyCharacter::Interactive()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	if (!HasAuthority())
	{
		Server_Interactive();
	}
	else if (HasAuthority() || IsRunningDedicatedServer())
	{
		Multi_Interactive();
	}
}

void AMyCharacter::Attack(const float Value)
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	if (Value == 0.f)
	{
		return;
	}

	if (!CanAttack)
	{
		return;
	}

	if (!HasAuthority())
	{
		Server_Attack(Value);
	}
	else if (HasAuthority() || IsRunningDedicatedServer())
	{
		Multi_Attack(Value);
	}
}

void AMyCharacter::Multi_Attack_Implementation(const float Value)
{
	AttackStart(Value);
}

void AMyCharacter::AttackStart(const float Value)
{
	if (Value == 0.f)
	{
		return;
	}

	constexpr int32 MaxAttackIndex = 3;

	if (IsValid(Weapon))
	{
		if (!Weapon->Attack())
		{
			return;
		}

		// todo: process client before sending rpc to server.

		switch (Weapon->GetWeaponStatComponent()->GetWeaponType())
		{
		case EMyWeaponType::Range:
			UE_LOG(LogTemp, Warning, TEXT("Range Attack"));
			// todo: unbind the fire when player drops.
			OnAttackEndedHandle = Weapon->BindOnFireReady(this, &AMyCharacter::ResetAttack);

			if (Weapon->GetWeaponStatComponent()->IsHitscan())
			{
				UE_LOG(LogTemp, Warning, TEXT("Hitscan Attack"));
				HitscanAttack();
			}
			break;
		case EMyWeaponType::Melee:
			MeleeAttack();
			break;
		default:
		case EMyWeaponType::Unknown: 
			UE_LOG(LogTemp, Error, TEXT("Unknown Weapon Type"));
			return;
		}
	}
	else
	{
		MeleeAttack();
	}

	CanAttack = false;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
}

void AMyCharacter::ResetAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Reset Attack"));
	CanAttack = true;
	OnAttackEnded.Broadcast();

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	Weapon->UnbindOnFireReady(OnAttackEndedHandle);
}

void AMyCharacter::Server_Interactive_Implementation()
{
	Multi_Interactive();
}

void AMyCharacter::Multi_Interactive_Implementation()
{
	InteractiveStart();
}

void AMyCharacter::InteractiveStart()
{
	UE_LOG(LogTemp, Warning, TEXT("Interactive"));
	FHitResult HitResult;
	const FCollisionQueryParams Params(NAME_None, false, this);
	TArray<FOverlapResult> HitResults;

	const bool Result = GetWorld()->OverlapMultiByChannel
	(
		OUT HitResults,
		GetActorLocation(),
		FQuat::Identity,
		ECC_EngineTraceChannel3,
		FCollisionShape::MakeSphere(100.f),
		Params
	);

	if (Result)
	{
		for (const auto& Hit : HitResults)
		{
			const auto& Interactive = Cast<AMyCollectable>(Hit.GetActor());

			if (IsValid(Interactive))
			{
				if (Interactive->GetItemOwner() != this && Interactive->Interact(this))
				{
					break;
				}
			}
		}
	}
}

void AMyCharacter::InteractInterrupted()
{
	if (!HasAuthority())
	{
		Server_InteractInterrupted();
	}
	else if (HasAuthority() || IsRunningDedicatedServer())
	{
		Multi_InteractInterrupted();
	}
}

void AMyCharacter::Server_InteractInterrupted_Implementation()
{
	Multi_InteractInterrupted();
}

void AMyCharacter::Multi_InteractInterrupted_Implementation()
{
	InteractInterruptedStart();
}

void AMyCharacter::InteractInterruptedStart() const
{
	UE_LOG(LogTemp, Warning, TEXT("Interact Interrupted"));
	OnInteractInterrupted.Broadcast();
}

void AMyCharacter::Use()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	if (!HasAuthority())
	{
		Server_Use();
	}
	else if (HasAuthority() || IsRunningDedicatedServer())
	{
		Multi_Use();
	}
}

void AMyCharacter::Server_Use_Implementation()
{
	Multi_Use();
}

void AMyCharacter::Multi_Use_Implementation()
{
	UseStart();
}

void AMyCharacter::UseStart()
{
	if (IsValid(CurrentItem))
	{
		CurrentItem->Use(this);
	}
	else
	{
		CurrentItem = Inventory->Use(0);
	}
}

void AMyCharacter::UseInterrupt()
{
	if (!HasAuthority())
	{
		Server_UseInterrupt();
	}
	else if (HasAuthority() || IsRunningDedicatedServer())
	{
		Multi_UseInterrupt();
	}
}

void AMyCharacter::Server_UseInterrupt_Implementation()
{
	Multi_UseInterrupt();
}

void AMyCharacter::Multi_UseInterrupt_Implementation()
{
	UseInterruptStart();
}

void AMyCharacter::UseInterruptStart() const
{
	UE_LOG(LogTemp, Warning, TEXT("Use Interrupted"));
	OnUseInterrupted.Broadcast();
}

int32 AMyCharacter::GetDamage() const
{
	if (IsValid(Weapon))
	{
		return StatComponent->GetDamage() + Weapon->GetDamage();
	}

	return StatComponent->GetDamage();
}

void AMyCharacter::OnAttackAnimNotify()
{
	FHitResult HitResult;
	// 포인터를 직접 비교하기보단 세번째 인자 InIgnoreActors로 본인을 제외할 수 있음
	FCollisionQueryParams Params(NAME_None, false, this);

	constexpr float AttackRange = 100.f;
	constexpr float AttackRadius = 50.f;

	FVector AttackEndVec = GetActorForwardVector() * AttackRange;
	constexpr FConstantFVector UpCompensation = FConstantFVector::UpVector * 50.f;
	constexpr FConstantFVector ForwardCompensation = FConstantFVector::ForwardVector * 25.f;
	FVector Center = GetActorLocation() + ForwardCompensation + (AttackEndVec * 0.5f) + (UpCompensation * 0.5f);
	float HalfHeight = AttackRange * 0.5f + AttackRadius;

	bool Result = GetWorld()->SweepSingleByChannel
	(
		OUT HitResult, 
		Center,
		GetActorLocation() + AttackEndVec,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2, // DefaultEngine.ini 참조
		FCollisionShape::MakeSphere(AttackRadius),
		Params // 까먹지 않도록 조심, 기본인자가 있음
	);

	bool ActualHit = Result && HitResult.Actor.IsValid();

	if (ActualHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.Actor->GetName());

		FDamageEvent DamageEvent;
		HitResult.Actor->TakeDamage
		(
			GetDamage(), 
			DamageEvent, 
			GetController(),
			this
		);
	}

	DrawDebugSphere
	(
		GetWorld(), 
		Center,
		HalfHeight,
		32,
		ActualHit ? FColor::Green : FColor::Red,
		false,
		1.f
	);
}

void AMyCharacter::Yaw(const float Value)
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	// 폰의 설정에서 Rotation Yaw가 true여야 함
	AddControllerYawInput(Value);
}

void AMyCharacter::Pitch(const float Value)
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	// 폰의 설정에서 Rotation Pitch가 true여야 함
	//AddControllerPitchInput(Value);	
}

bool AMyCharacter::IsBuyMenuOpened() const
{
	const auto& HUD = Cast<AMyInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	if (HUD)
	{
		return HUD->IsBuyMenuOpened();
	}

	return false;
}
