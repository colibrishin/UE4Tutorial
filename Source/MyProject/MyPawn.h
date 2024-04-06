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
	// �������Ʈ���� �Լ��� ����Ϸ��� UFUNCTION() ��ũ�θ� ����ؾ���
	
	void UpDown(const float Value);

	void LeftRight(const float Value);

	// UProperties, �����Ϳ� �ش� ������Ʈ �� ������ ���� ���ִ� ��Ȱ
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	// �⺻������ ����Ʈ�� ����� ����, �ش� ������Ʈ�� ���Ե����� �ʱ� ������, ���漱���� �ϰ� �����ο��� ���� ��ü�� ������ ����Ͽ� incomplete type�� ���� �ʵ��� ��.
	// + �߷� ȿ���� ���� ������Ʈ
	UPROPERTY(VisibleAnywhere)
	class UFloatingPawnMovement* Movement;

};
