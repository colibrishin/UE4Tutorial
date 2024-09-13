// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "DrawDebugHelpers.h"
#include "MyAIController.h"
#include "MyAimableWeapon.h"
#include "MyAnimInstance.h"
#include "MyC4.h"
#include "MyInGameHUD.h"
#include "MyPlayerController.h"
#include "MyWeapon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/C_PickUp.h"
#include "Components/MyInventoryComponent.h"
#include "Components/MyStatComponent.h"
#include "Components/MyWeaponStatComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Widgets/MyCharacterWidget.h"

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
	ArmMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMeshComponent"));

	// 리소스를 불러오는 방법
	static ConstructorHelpers::FObjectFinder<USoundWave> SW_Footstep(TEXT("SoundWave'/Game/Models/sounds/footstep1.footstep1'"));

	if (SW_Footstep.Succeeded())
	{
		FootstepSound = SW_Footstep.Object;
	}

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	ArmMeshComponent->SetupAttachment(Camera);

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

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetCastShadow(true);
	ArmMeshComponent->SetOnlyOwnerSee(true);
	ArmMeshComponent->SetCastShadow(false);
}

USkeletalMeshComponent* AMyCharacter::GetArmMeshComponent() const
{
	return ArmMeshComponent;
}

UMyInventoryComponent* AMyCharacter::GetInventory() const
{
	const auto& State = GetPlayerState<AMyPlayerState>();
	return State->GetInventoryComponent();
}

UMyStatComponent*      AMyCharacter::GetStatComponent() const
{
	if (const AMyPlayerState* State = GetPlayerState<AMyPlayerState>())
	{
		return State->GetStatComponent();	
	}

	return nullptr;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMyCharacter::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState , OldPlayerState);
	
	if (const AMyPlayerState* MyPlayerState = GetPlayerState<AMyPlayerState>())
	{
		if (const UMyStatComponent* StatComponent = MyPlayerState->GetStatComponent())
		{
			LOG_FUNC_PRINTF(LogTemp, Log, "Player data asset updated: %s, Client? : %d", *GetName(), GetNetMode() == NM_Client);
			GetMesh()->SetSkeletalMesh(StatComponent->GetSkeletalMesh());
			GetMesh()->SetAnimInstanceClass(StatComponent->GetAnimInstance());
			GetArmMeshComponent()->SetSkeletalMesh(StatComponent->GetArmSkeletalMesh());
			GetArmMeshComponent()->SetAnimInstanceClass(StatComponent->GetArmAnimInstance());
			UpdateMeshAnimInstance();
			UpdateArmMeshAnimInstance();
		}
		else
		{
			LOG_FUNC_PRINTF(LogTemp, Error, "Unable to get the stat componenet : %s", *MyPlayerState->GetName());
		}
	}
	else
	{
		LOG_FUNC_PRINTF(LogTemp, Error, "Unable to get player state : %s", *GetName());
	}
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCharacter, PitchInput);
	DOREPLIFETIME(AMyCharacter, bHandBusy);
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

void AMyCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FootstepSound, GetActorLocation(), FRotator::ZeroRotator);
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

	if (Value == 0.f)
	{
		return;
	}

	Multi_MeleeAttack();

	OnAttackStarted.Broadcast();

	CanAttack = false;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	PreviousAttack = Value;

	Client_Attack();
}


void AMyCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AnimInstance->GetAttackMontage())
	{
		ResetAttack();
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

void AMyCharacter::Multi_MeleeAttack_Implementation()
{
	constexpr int32 MaxAttackSection = 3;

	UE_LOG(LogTemp, Warning, TEXT("Melee Attack"));
	AttackIndex = (AttackIndex + 1) % MaxAttackSection;
	AnimInstance->PlayAttackMontage(AttackIndex);
	ArmAnimInstance->PlayAttackMontage(AttackIndex);
}

void AMyCharacter::Client_Attack_Implementation()
{
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
}

void AMyCharacter::UpdateMeshAnimInstance()
{
	AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &AMyCharacter::OnMontageEnded);
	AnimInstance->OnAttackHit.AddUniqueDynamic(this, &AMyCharacter::OnAttackAnimNotify);
}

void AMyCharacter::UpdateArmMeshAnimInstance()
{
	ArmAnimInstance = Cast<UMyAnimInstance>(GetArmMeshComponent()->GetAnimInstance());
	ArmAnimInstance->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnMontageEnded);
}

void AMyCharacter::OnAttackAnimNotify()
{
	TArray<FHitResult> HitResults;
	// 포인터를 직접 비교하기보단 세번째 인자 InIgnoreActors로 본인을 제외할 수 있음
	FCollisionQueryParams Params(NAME_None, false, this);

	constexpr float AttackRange = 100.f;
	constexpr float AttackRadius = 50.f;

	FVector AttackEndVec = GetActorForwardVector() * AttackRange;
	const FVector UpCompensation = FVector::UpVector * 50.f;
	const FVector ForwardCompensation = FVector::ForwardVector * 25.f;
	FVector Center = GetActorLocation() + ForwardCompensation + (AttackEndVec * 0.5f) + (UpCompensation * 0.5f);
	float HalfHeight = AttackRange * 0.5f + AttackRadius;

	bool Result = GetWorld()->SweepMultiByChannel
	(
		OUT HitResults, 
		Center,
		GetActorLocation() + AttackEndVec,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2, // DefaultEngine.ini 참조
		FCollisionShape::MakeSphere(AttackRadius),
		Params // 까먹지 않도록 조심, 기본인자가 있음
	);
	
	FHitResult FirstHit;

	for (const auto& Element : HitResults)
	{
		if (const auto& CastTest = Cast<AMyCharacter>(Element.GetActor()))
		{
			if (IsValid(CastTest->GetPlayerState<AMyPlayerState>()))
			{
				FirstHit = Element;
				break;
			}
		}
	}

	const bool ActualHit = Result && IsValid(FirstHit.GetActor());
	
	if (ActualHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *FirstHit.GetActor()->GetName());

		const FDamageEvent DamageEvent;
		FirstHit.GetActor()->TakeDamage
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

void AMyCharacter::PickUp(UC_PickUp* InPickUp)
{
	IPickableObject::PickUp(InPickUp);

	if (bHandBusy)
	{
		// todo: move to inventory;
		return; 
	}
	
	GetPlayerState<AMyPlayerState>()->SetHand(InPickUp);

	if (USkeletalMeshComponent* ObjectMesh = InPickUp->GetOwner()->GetComponentByClass<USkeletalMeshComponent>())
	{
		ObjectMesh->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			RightHandSocketName);

		bHandBusy = true;
	}
}

void AMyCharacter::Server_SyncPitch_Implementation(const float NewPitch)
{
	PitchInput = NewPitch;
}
