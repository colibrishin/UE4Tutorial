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
#include "NiagaraComponent.h"

const FName AMyCharacter::LeftHandSocketName(TEXT("hand_l_socket"));
const FName AMyCharacter::RightHandSocketName(TEXT("hand_r_socket"));
const FName AMyCharacter::HeadSocketName(TEXT("head_socket"));
const FName AMyCharacter::ChestSocketName(TEXT("Chest"));

// Sets default values
AMyCharacter::AMyCharacter() : CanAttack(true)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	// 리소스를 불러오는 방법
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mesh(TEXT("SkeletalMesh'/Game/ParagonBoris/Characters/Heroes/Boris/Meshes/Boris.Boris'"));

	if (SK_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Mesh.Object);
	}

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	// 기본 캡슐 사이즈, 매쉬가 붕 뜨지 않게 하도록
	// Character 구현부 52번 라인 참조
	// 캐릭터의 바닥을 맞추기 위해 사용할 수도 있음
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	SpringArm->TargetArmLength = 0.f;
	SpringArm->SetRelativeLocation({93.f, 41.f, 84.f});

	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = false;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;

	AttackIndex = 0;

	AIControllerClass = AMyAIController::StaticClass();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UMyInventoryComponent* AMyCharacter::GetInventory() const
{
	const auto& State = GetPlayerState<AMyPlayerState>();
	return State->GetInventoryComponent();
}

UMyStatComponent*      AMyCharacter::GetStatComponent() const
{
	const auto& State = GetPlayerState<AMyPlayerState>();
	return State->GetStatComponent();
}

AMyWeapon* AMyCharacter::GetWeapon() const
{
	const auto& State = GetPlayerState<AMyPlayerState>();

	if (!IsValid(State))
	{
		return nullptr;
	}

	return State->GetWeapon();
}

AMyCollectable* AMyCharacter::GetCurrentItem() const
{
	const auto& State = GetPlayerState<AMyPlayerState>();

	if (IsValid(State))
	{
		return State->GetCurrentItem();
	}

	return nullptr;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	const auto& MyPlayerState = GetPlayerState<AMyPlayerState>();

	if (IsValid(MyPlayerState))
	{
		MyPlayerState->BindOnWeaponChanged(this, &AMyCharacter::OnWeaponChanged);
	}
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

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCharacter, PitchInput);
}

float AMyCharacter::TakeDamage(
	float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser
)
{
	if (HasAuthority())
	{
		if (const auto& MyPlayerState = GetPlayerState<AMyPlayerState>())
		{
			MyPlayerState->TakeDamage
			(
				Damage,
				DamageEvent,
				EventInstigator,
				DamageCauser
			);
			UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), Damage);
		}
	}

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

	PlayerInputComponent->BindAction(TEXT("Interactive"), IE_Pressed, this, &AMyCharacter::Interactive);
	PlayerInputComponent->BindAction(TEXT("Interactive"), IE_Released, this, &AMyCharacter::InteractInterrupted);

	PlayerInputComponent->BindAction(TEXT("Use"), IE_Pressed, this, &AMyCharacter::Use);
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

void AMyCharacter::Server_Attack_Implementation(const float Value)
{
	if (!CanAttack)
	{
		return;
	}

	if (IsValid(GetWeapon()) && !GetWeapon()->CanDoAttack())
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

	ExecuteServer
	(
	 this,
		 &AMyCharacter::Server_Reload,
		 &AMyCharacter::Multi_Reload
	);
}

void AMyCharacter::Server_Reload_Implementation()
{
	if (!IsValid(GetWeapon()))
	{
		return;
	}

	if (!GetWeapon()->CanBeReloaded())
	{
		return;
	}

	Multi_Reload();
}

void AMyCharacter::NotifyDamage(AMyCharacter* const Target) const
{
	if (IsValid(Target))
	{
		UE_LOG(LogTemp, Warning , TEXT("Hit Actor: %s"), *Target->GetName());
		const FDamageEvent DamageEvent;
		Target->TakeDamage(GetDamage(), DamageEvent, GetController(), GetWeapon());
	}
}

bool AMyCharacter::HitscanAttack(OUT FHitResult& OutHitResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Hitscan Fire!"));

	FCollisionQueryParams Params{NAME_None, false, this};

	const FVector EndVector = 
		Camera->GetComponentLocation() + 
		Camera->GetForwardVector() * 
		GetWeapon()->GetWeaponStatComponent()->GetRange();

	const auto& Result = GetWorld()->LineTraceSingleByChannel
		(
		 OUT OutHitResult,
		 Camera->GetComponentLocation() ,
		 EndVector,
		 ECollisionChannel::ECC_Visibility,
		 Params
		);

	return Result;
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
	if (!IsValid(GetWeapon()))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Reload"));
	GetWeapon()->Reload();
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

	if (!IsValid(GetWeapon()))
	{
		return;
	}

	if (!GetWeapon()->IsA<AMyAimableWeapon>())
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

	ExecuteServer
	(
	 this,
		 &AMyCharacter::Server_Interactive,
		 &AMyCharacter::InteractiveImpl
	);
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

	ExecuteServer
		(
		 this,
		 &AMyCharacter::Server_Attack,
		 &AMyCharacter::Multi_Attack,
		 Value
		);
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

	if (IsValid(GetWeapon()))
	{
		LOG_FUNC_RAW(LogTemp, Warning, *FString::Printf(TEXT("Attack with weapon, Is Client? : %d"), !HasAuthority()));

		if (!GetWeapon()->Attack())
		{
			LOG_FUNC(LogTemp, Error, "Failed to attack");
			return;
		}

		// todo: process client before sending rpc to server.

		switch (GetWeapon()->GetWeaponStatComponent()->GetWeaponType())
		{
		case EMyWeaponType::Range:
			UE_LOG(LogTemp, Warning, TEXT("Range Attack"));
			// todo: unbind the fire when player drops.
			OnAttackEndedHandle = GetWeapon()->BindOnFireReady(this, &AMyCharacter::ResetAttack);

			if (GetWeapon()->GetWeaponStatComponent()->IsHitscan())
			{
				UE_LOG(LogTemp, Warning, TEXT("Hitscan Attack"));

				FHitResult HitResult;
				if (HitscanAttack(HitResult))
				{
					NotifyDamage(Cast<AMyCharacter>(HitResult.Actor.Get()));
				}
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
		LOG_FUNC_RAW(LogTemp, Warning, *FString::Printf(TEXT("Attack without weapon, Is Client? : %d"), HasAuthority()));
		MeleeAttack();
	}

	OnAttackStarted.Broadcast();

	CanAttack = false;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
}

void AMyCharacter::ResetAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Reset Attack"));
	CanAttack = true;
	OnAttackEnded.Broadcast();

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetWeapon()->UnbindOnFireReady(OnAttackEndedHandle);
}

void AMyCharacter::Server_Interactive_Implementation()
{
	InteractiveImpl();
}

void AMyCharacter::InteractiveImpl()
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
		ECC_GameTraceChannel9,
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
				if (Interactive->GetItemOwner() != this)
				{
					Interactive->Interact(this);
					break;
				}
			}
		}
	}
}

void AMyCharacter::InteractInterrupted()
{
	ExecuteServer
		(
		 this,
		 &AMyCharacter::Server_InteractInterrupted,
		 &AMyCharacter::InteractInterruptedImpl
		);
}

void AMyCharacter::Server_InteractInterrupted_Implementation()
{
	InteractInterruptedImpl();
}

void AMyCharacter::InteractInterruptedImpl() const
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

	ExecuteServer
		(
		 this,
		 &AMyCharacter::Server_Use,
		 &AMyCharacter::UseImpl
		);
}

void AMyCharacter::Server_Use_Implementation()
{
	UseImpl();
}

void AMyCharacter::UseImpl()
{
	if (IsValid(GetCurrentItem()))
	{
		GetCurrentItem()->Use(this);
	}
	else
	{
		GetPlayerState<AMyPlayerState>()->Use(0);
	}
}

void AMyCharacter::UseInterrupt()
{
	ExecuteServer
		(
		 this,
		 &AMyCharacter::Server_UseInterrupt,
		 &AMyCharacter::UseInterruptImpl
		);
}

void AMyCharacter::Server_UseInterrupt_Implementation()
{
	UseInterruptImpl();
}

void AMyCharacter::UseInterruptImpl() const
{
	UE_LOG(LogTemp, Warning, TEXT("Use Interrupted"));
	OnUseInterrupted.Broadcast();
}

void AMyCharacter::OnWeaponChanged(AMyPlayerState* ThisPlayerState) const
{
	const auto& Weapon = ThisPlayerState->GetWeapon();

	if (IsValid(Weapon))
	{
		Weapon->AttachToComponent
		(
			GetMesh(), 
			FAttachmentTransformRules::SnapToTargetIncludingScale, 
			AMyCharacter::RightHandSocketName
		);
	}
	else
	{
		for (const auto& Child : GetMesh()->GetAttachChildren())
		{
			if (Child->GetAttachSocketName() == AMyCharacter::RightHandSocketName)
			{
				Child->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}

int32 AMyCharacter::GetDamage() const
{
	const auto& State = GetPlayerState<AMyPlayerState>();

	if (!IsValid(State))
	{
		return 0;
	}

	if (IsValid(GetWeapon()))
	{
		return State->GetDamage() + GetWeapon()->GetDamage();
	}

	return State->GetDamage();
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

	if (Value == 0.f)
	{
		return;
	}

	// 폰의 설정에서 Rotation Pitch가 true여야 함
	AddControllerPitchInput(-Value);

	if (!HasAuthority())
	{
		Server_SyncPitch(GetControlRotation().Pitch);
	}
	else
	{
		PitchInput = GetControlRotation().Pitch;
	}
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

void AMyCharacter::Server_SyncPitch_Implementation(const float NewPitch)
{
	PitchInput = NewPitch;
}
