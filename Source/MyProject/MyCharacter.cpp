// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "MyAnimInstance.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "DrawDebugHelpers.h"
#include "MyAIController.h"
#include "MyAimableWeapon.h"
#include "MyC4.h"
#include "MyCharacterWidget.h"
#include "MyInGameHUD.h"
#include "MyInventoryComponent.h"
#include "MyStatComponent.h"
#include "MyWeapon.h"

#include "Components/WidgetComponent.h"

#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"

#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"

#include "Kismet/KismetMathLibrary.h"

#include "UObject/ConstructorHelpers.h"

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
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mesh(TEXT("SkeletalMesh'/Game/ParagonBoris/Characters/Heroes/Boris/Meshes/Boris.Boris'"));

	if (SK_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Mesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_ArmMesh(TEXT("SkeletalMesh'/Game/Models/Boris_Arms/Boris_arms.Boris_arms'"));

	if (SK_ArmMesh.Succeeded())
	{
		ArmMeshComponent->SetSkeletalMesh(SK_ArmMesh.Object);
	}

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

AMyWeapon* AMyCharacter::TryGetWeapon() const
{
	const auto& MyPlayerState = GetPlayerState<AMyPlayerState>();

	if (IsValid(MyPlayerState))
	{
		return Cast<AMyWeapon>(MyPlayerState->GetCurrentHand());
	}
	else
	{
		return nullptr;
	}
}

AMyItem* AMyCharacter::TryGetItem() const
{
	const auto& MyPlayerState = GetPlayerState<AMyPlayerState>();

	if (IsValid(MyPlayerState))
	{
		return Cast<AMyItem>(MyPlayerState->GetCurrentHand());
	}
	else
	{
		return nullptr;
	}
}

AMyCollectable*         AMyCharacter::GetCurrentHand() const
{
	return HandCollectable;
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
	const auto& State = GetPlayerState<AMyPlayerState>();
	return State->GetStatComponent();
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
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

	const auto& ArmAnim = ArmMeshComponent->GetAnimInstance();

	if (IsValid(ArmAnim))
	{
		ArmAnimInstance = Cast<UMyAnimInstance>(ArmAnim);
		ArmAnimInstance->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnMontageEnded);
	}
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCharacter, PitchInput);
	DOREPLIFETIME(AMyCharacter, HandCollectable);
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

	PlayerInputComponent->BindAction(TEXT("Interactive"), IE_Pressed, this, &AMyCharacter::Interactive);
	PlayerInputComponent->BindAction(TEXT("Interactive"), IE_Released, this, &AMyCharacter::InteractInterrupted);

	PlayerInputComponent->BindAction(TEXT("Use"), IE_Pressed, this, &AMyCharacter::Use);
	PlayerInputComponent->BindAction(TEXT("Use"), IE_Released, this, &AMyCharacter::UseInterrupt);

	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Repeat, this, &AMyCharacter::Aim);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &AMyCharacter::UnAim);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AMyCharacter::Reload);

	PlayerInputComponent->BindAction(TEXT("Primary"), IE_Pressed, this, &AMyCharacter::SwapPrimary);
	PlayerInputComponent->BindAction(TEXT("Secondary"), IE_Pressed, this, &AMyCharacter::SwapSecondary);
	PlayerInputComponent->BindAction(TEXT("Melee"), IE_Pressed, this, &AMyCharacter::SwapMelee);
	PlayerInputComponent->BindAction(TEXT("Grenade"), IE_Pressed, this, &AMyCharacter::SwapUtility);
	PlayerInputComponent->BindAction(TEXT("Bomb"), IE_Pressed, this, &AMyCharacter::SwapBomb);

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

	if (IsValid(TryGetWeapon()) && !TryGetWeapon()->CanDoAttack())
	{
		return;
	}

	if (Value == 0.f)
	{
		return;
	}

	if (IsValid(TryGetWeapon()))
	{
		LOG_FUNC_RAW(LogTemp, Warning, *FString::Printf(TEXT("Attack with weapon, Is Client? : %d"), !HasAuthority()));

		if (TryGetWeapon()->Attack())
		{
			if (const auto& HandWeapon = Cast<AMyWeapon>(HandCollectable))
			{
				HandWeapon->Attack();
				//HandWeapon->BindOnFireReady(this, &AMyCharacter::ResetAttack);
			}
		}
		else
		{
			LOG_FUNC(LogTemp, Error, "Failed to attack");
			return;
		}

		// todo: process client before sending rpc to server.

		if (IsValid(TryGetWeapon()))
		{
			switch (TryGetWeapon()->GetWeaponStatComponent()->GetWeaponType())
			{
			case EMyWeaponType::Range:
				LOG_FUNC(LogTemp, Warning, "Range Attack");
				break;
			case EMyWeaponType::Melee:
				LOG_FUNC(LogTemp, Warning, "Melee Attack");
				Multi_MeleeAttack();
				break;
			default:
			case EMyWeaponType::Unknown:
				LOG_FUNC(LogTemp, Error, "Unknown Weapon Type");
				Multi_MeleeAttack();
				break;
			}
		}
	}
	else
	{
		LOG_FUNC_RAW(LogTemp, Warning, *FString::Printf(TEXT("Attack without weapon, Is Client? : %d"), HasAuthority()));
		Multi_MeleeAttack();
	}

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

void AMyCharacter::Reload()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	if (!IsValid(TryGetWeapon()))
	{
		return;
	}

	if (!TryGetWeapon()->CanBeReloaded())
	{
		return;
	}

	Server_Reload();
}

void AMyCharacter::Server_AttackInterrupted_Implementation(const float Value)
{
	PreviousAttack = Value;

	// Will not reset attack here. There could be a case where player is doing the short burst shooting.
	if (IsValid(TryGetWeapon()))
	{
		TryGetWeapon()->AttackInterrupted();
	}
}

void AMyCharacter::Server_Reload_Implementation()
{
	if (!IsValid(TryGetWeapon()))
	{
		return;
	}

	if (!TryGetWeapon()->CanBeReloaded())
	{
		return;
	}

	if (!IsValid(TryGetWeapon()))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Reload"));
	TryGetWeapon()->Reload();
	Client_Reload();
}

void AMyCharacter::Client_Reload_Implementation()
{
	if (IsValid(HandCollectable))
	{
		if (const auto& HandWeapon = Cast<AMyWeapon>(HandCollectable))
		{
			HandWeapon->Reload();
		}
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
	if (IsBuyMenuOpened())
	{
		return;
	}

	if (!IsValid(TryGetWeapon()))
	{
		return;
	}

	if (!TryGetWeapon()->IsA<AMyAimableWeapon>())
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

	Server_Interactive();
}

void AMyCharacter::Server_Interactive_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Interactive"));
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
					Interactive->Server_Interact(this);
					break;
				}
			}
		}
	}
}

void AMyCharacter::Attack(const float Value)
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	// todo: analogue input support?
	if (PreviousAttack == 1.f && Value == 0.f)
	{
		PreviousAttack = Value;
		Server_AttackInterrupted(Value);
		return;
	}

	PreviousAttack = Value;

	if (Value == 0.f)
	{
		return;
	}

	if (!CanAttack)
	{
		LOG_FUNC(LogTemp, Log, "Unable to attack.");
		return;
	}

	Server_Attack(Value);
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

void AMyCharacter::InteractInterrupted()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	Server_InteractInterrupted();
}

void AMyCharacter::Server_InteractInterrupted_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Server Interact Interrupted"));
	OnInteractInterrupted.Broadcast();

	Client_InteractInterrupted();
}

void AMyCharacter::Client_InteractInterrupted_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Client Interact Interrupted"));
	OnInteractInterrupted.Broadcast();
}

void AMyCharacter::Use()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	if (!IsValid(TryGetItem()))
	{
		return;
	}

	TryGetItem()->Server_Use(this);
}

void AMyCharacter::UseInterrupt()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	if (!IsValid(TryGetItem()))
	{
		return;
	}

	Server_UseInterrupt();
}

void AMyCharacter::Server_UseInterrupt_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Server Use Interrupted"));
	OnUseInterrupted.Broadcast();

	Client_UseInterrupted();
}

void AMyCharacter::OnHandChanged(AMyCollectable* Previous, AMyCollectable* New, AMyPlayerState* ThisPlayerState)
{
	LOG_FUNC(LogTemp, Warning, "Weapon change caught");

	if (IsValid(New))
	{
		if (IsValid(Previous))
		{
			if (const auto& WeaponCast = Cast<AMyWeapon>(Previous);
				IsValid(WeaponCast))
			{
				WeaponCast->OnFireReady.RemoveDynamic(WeaponCast, &AMyWeapon::OnFireRateTimed);
			}
		}
		
		New->AttachToComponent
		(
			GetMesh(), 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
			AMyCharacter::RightHandSocketName
		);

		if (HasAuthority())
		{
			AttachArmCollectable(Previous, New);
		}
	}
	else
	{
	    // Defensive condition for dropping weapon.
		if (Previous->GetItemOwner() == this)
		{
			Previous->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

			Previous->AttachToComponent
			(
			  GetMesh(),
			  FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			  AMyCharacter::ChestSocketName
			);

			Previous->Hide();
		}

		if (IsValid(HandCollectable))
		{
			HandCollectable->Destroy(true);
			HandCollectable = nullptr;
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

	if (IsValid(TryGetWeapon()))
	{
		return State->GetDamage() + TryGetWeapon()->GetDamage();
	}

	return State->GetDamage();
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

void AMyCharacter::Client_UseInterrupted_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Use Interrupted"));
	OnUseInterrupted.Broadcast();
}

void AMyCharacter::AttachArmCollectable(class AMyCollectable* Previous, class AMyCollectable* New)
{
	LOG_FUNC(LogTemp, Warning, "AttachArmCollectable");

	if (IsValid(HandCollectable))
	{
		HandCollectable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HandCollectable->Destroy();
		HandCollectable = nullptr;
	}

	if (IsValid(New))
	{
		HandCollectable = GetWorld()->SpawnActor<AMyCollectable>(New->GetClass());
		HandCollectable->GetMesh()->SetSimulatePhysics(false);

		if (HandCollectable->GetMesh()->AttachToComponent
		(
			ArmMeshComponent,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			AMyCharacter::RightHandSocketName
		))
		{
			HandCollectable->SetOwner(this);
			HandCollectable->SetReplicates(true);
			HandCollectable->GetMesh()->SetVisibility(true);
			HandCollectable->GetMesh()->SetOnlyOwnerSee(true);
			HandCollectable->GetMesh()->SetCastShadow(false);

			if (const auto& Weapon = Cast<AMyWeapon>(HandCollectable))
			{
				Weapon->SetVisualDummy(true);
			}

			LOG_FUNC(LogTemp, Warning, "Weapon attached to arm");
		}
		else
		{
			LOG_FUNC(LogTemp, Error, "Failed to attach weapon to arm");
		}
	}
}

// todo: Meta-programming?

void AMyCharacter::SwapPrimary()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	Server_SwapPrimary();
}

void AMyCharacter::Server_SwapPrimary_Implementation()
{
	WeaponSwap(1);
}

void AMyCharacter::SwapSecondary()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	Server_SwapSecondary();
}

void AMyCharacter::Server_SwapSecondary_Implementation()
{
	WeaponSwap(2);
}

void AMyCharacter::SwapMelee()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	Server_SwapMelee();
}

void AMyCharacter::Server_SwapMelee_Implementation()
{
	WeaponSwap(3);
}

void AMyCharacter::SwapUtility()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	Server_SwapUtility();
}

void AMyCharacter::Server_SwapUtility_Implementation()
{
	WeaponSwap(4);
}

void AMyCharacter::SwapBomb()
{
	if (IsBuyMenuOpened())
	{
		return;
	}

	Server_SwapBomb();
}

void AMyCharacter::Server_SwapBomb_Implementation()
{
	WeaponSwap(5);
}

void AMyCharacter::WeaponSwap(const int32 Index) const
{
	const auto& Inventory = GetInventory();

	if (const auto& Collectable = Inventory->Get(Index))
	{
		const auto& ThisPlayerState = GetPlayerState<AMyPlayerState>();

		if (Collectable == ThisPlayerState->GetCurrentHand())
		{
			return;
		}

		if (IsValid(Collectable))
		{
			ThisPlayerState->SetCurrentItem(Collectable);
		}
	}
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
	if (const auto& HUD = Cast<AMyInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		return HUD->IsBuyMenuOpened();
	}

	return false;
}

void AMyCharacter::Server_SyncPitch_Implementation(const float NewPitch)
{
	PitchInput = NewPitch;
}
