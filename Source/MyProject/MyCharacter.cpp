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
#include "MyCharacterWidget.h"
#include "MyInventoryComponent.h"

#include "Components/WidgetComponent.h"

const FName AMyCharacter::LeftHandSocketName(TEXT("hand_l_socket"));
const FName AMyCharacter::RightHandSocketName(TEXT("hand_r_socket"));
const FName AMyCharacter::HeadSocketName(TEXT("head_socket"));

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

	SpringArm->TargetArmLength = 100.0f;
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
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AMyCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Interactive"), IE_Pressed, this, &AMyCharacter::Interactive);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &AMyCharacter::Aim);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &AMyCharacter::UnAim);

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
			AimableWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, RightHandSocketName);
			AimableWeapon->BindOnFireReady([this]()
			{
				CanAttack = true;
			});

			return true;
		}

		UE_LOG(LogTemp, Warning, TEXT("Weapon: %s"), *Weapon->GetName());
		Weapon->Show();
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, LeftHandSocketName);
		return true;
	}

	return false;
}

void AMyCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AnimInstance->GetAttackMontage())
	{
		CanAttack = true;
		OnAttackEnded.Broadcast();
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
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
	if (!Weapon->IsA<AMyAimableWeapon>())
	{
		return;
	}

	if (IsAiming)
	{
		return;
	}

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

void AMyCharacter::Attack()
{
	if (!CanAttack)
	{
		return;
	}

	if (IsValid(Weapon))
	{
		if (Weapon->IsA<AMyAimableWeapon>())
		{
			UE_LOG(LogTemp , Warning , TEXT("AimableWeapon Attack"));
			const auto& AimableWeapon = Cast<AMyAimableWeapon>(Weapon);

			if (IsValid(Weapon))
			{
				AimableWeapon->Fire();
				CanAttack = false;
				return;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Melee Attack"));

	constexpr int32 MaxAttackIndex = 3;

	AttackIndex = (AttackIndex + 1) % MaxAttackIndex;
	AnimInstance->PlayAttackMontage(AttackIndex);
	CanAttack = false;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
}

void AMyCharacter::Interactive()
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
			const auto& Collectable = Cast<AMyInteractiveActor>(Hit.GetActor());

			if (IsValid(Collectable))
			{
				if (Collectable->Interact(this))
				{
					break;
				}
			}
		}
	}
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
		ECollisionChannel::ECC_EngineTraceChannel2, // DefaultEngine.ini 참조
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
	// 폰의 설정에서 Rotation Yaw가 true여야 함
	AddControllerYawInput(Value);
}

void AMyCharacter::Pitch(const float Value)
{
	// 폰의 설정에서 Rotation Pitch가 true여야 함
	//AddControllerPitchInput(Value);	
}
