// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeapon.h"

#include "MyCharacter.h"

#include "Components/BoxComponent.h"

// Sets default values
AMyWeapon::AMyWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Spline(TEXT("StaticMesh'/Game/ParagonBoris/FX/Meshes/Splines/SM_Forward_Burst_Splines.SM_Forward_Burst_Splines'"));

	if (SM_Spline.Succeeded())
	{
		Mesh->SetStaticMesh(SM_Spline.Object);
	}

	Mesh->SetupAttachment(RootComponent);
	Collider->SetupAttachment(Mesh);

	Mesh->SetCollisionProfileName(TEXT("MyCollectable"));
	Collider->SetCollisionProfileName(TEXT("MyCollectable"));

	Collider->SetBoxExtent(FVector{10.f, 30.f, 10.f});
}

// Called when the game starts or when spawned
void AMyWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Collider->OnComponentBeginOverlap.AddDynamic(this, &AMyWeapon::OnCharacterOverlap);
}

void AMyWeapon::OnCharacterOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult
)
{
	AMyCharacter* const MyCharacter = Cast<AMyCharacter>(OtherActor);

	if (IsValid(MyCharacter))
	{
		MyCharacter->TryPickWeapon(this);
	}
}
