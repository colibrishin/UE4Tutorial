// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

UCLASS()
class MYPROJECT_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// 블루프린트에서 함수를 사용하려면 UFUNCTION() 매크로를 사용해야함
	
	void UpDown(const float Value);

	void LeftRight(const float Value);

	// UProperties, 에디터에 해당 컴포넌트 및 변수를 띄우게 해주는 역활
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	// 기본적으로 컴팩트한 헤더를 쓰고, 해당 컴포넌트가 포함돼있지 않기 때문에, 전방선언을 하고 구현부에서 실제 객체의 구현을 사용하여 incomplete type이 되지 않도록 함.
	// + 중력 효과가 없는 컴포넌트
	UPROPERTY(VisibleAnywhere)
	class UFloatingPawnMovement* Movement;

};
