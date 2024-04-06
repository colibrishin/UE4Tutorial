// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// TEXT() == L"" ó�� �����ڵ� ȯ�濡���� ����� �� �ִ� wchar_t�� ��ȯ�ϴ� ��Ȱ�� ��. �ѱ��� ���ϱ� ���� ���̴°� ���� ��.

	// �𸮾��� �����ϴ� ���� ������Ʈ ���� �Լ�
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// �׼ǿ��� �߻��ϴ� �̺�Ʈ�� �ڵ鸵�ϴ� �Լ��� ������. Delegation.

	// �� vs �׼�
	// �� = �Ƴ��α� ��
	// �׼� = ������ ��

	// �Լ� �����͸� �Ѱ��� ��, ��ü�� ������� ������ ��ü �Լ� ������ (void(type*, ...)) ���·� �Ѿ.
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyPawn::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyPawn::LeftRight);
}

void AMyPawn::UpDown(const float Value)
{
	// == Vector3{1, 0, 0} * acceleration
	Movement->AddInputVector(GetActorForwardVector() * Value);
}
void AMyPawn::LeftRight(const float Value)
{
	Movement->AddInputVector(GetActorRightVector() * Value);
}

