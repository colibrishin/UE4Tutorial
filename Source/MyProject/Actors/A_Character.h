#pragma once

#include "CoreMinimal.h"
#include "../Interfaces/PickableObject.h"
#include "GameFramework/Character.h"

#include "A_Character.generated.h"

DECLARE_LOG_CATEGORY_EXTERN( LogCharacter , Log, All );

class UInputMappingContext;

UCLASS()
class MYPROJECT_API AA_Character : public ACharacter, public IPickableObject
{
	GENERATED_BODY()

public:
	static const FName LeftHandSocketName;
	static const FName RightHandSocketName;
	static const FName HeadSocketName;
	static const FName ChestSocketName;

	// Sets default values for this character's properties
	AA_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PickUp(UC_PickUp* InPickUp) override;

	virtual void Drop(UC_PickUp* InPickUp) override;

	UInputMappingContext* InputMapping;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, Replicated)
	bool bHandBusy;
};
