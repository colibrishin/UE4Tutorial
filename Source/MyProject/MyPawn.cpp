// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// TEXT() == L"" 처럼 유니코드 환경에서도 사용할 수 있는 wchar_t로 변환하는 역활을 함. 한글을 쓰니까 습관 들이는게 좋을 듯.

	// 언리얼이 제공하는 하위 오브젝트 생성 함수
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

	// 액션에서 발생하는 이벤트를 핸들링하는 함수를 설정함. Delegation.

	// 축 vs 액션
	// 축 = 아날로그 값
	// 액션 = 디지털 값

	// 함수 포인터를 넘겨줄 때, 객체를 명시하지 않으면 객체 함수 포인터 (void(type*, ...)) 형태로 넘어감.
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

