// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyInGameHUD.h"

#include "MyAmmoWidget.h"
#include "MyBuyMenuWidget.h"
#include "MyCharacter.h"
#include "MyGameState.h"
#include "MyInGameWidget.h"
#include "MyPlayerState.h"
#include "MyStatComponent.h"

#include "Components/WidgetComponent.h"

AMyInGameHUD::AMyInGameHUD()
{
	Widgets = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widgets"));
	BuyMenu = CreateDefaultSubobject<UWidgetComponent>(TEXT("BuyMenu"));

	SetRootComponent(Widgets);
	Widgets->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> BP_Widget(TEXT("WidgetBlueprint'/Game/Blueprints/UIs/BPMyInGameUIWidget.BPMyInGameUIWidget_C'"));

	if (BP_Widget.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget is loaded"));
		Widgets->SetWidgetClass(BP_Widget.Class);
	}

	static ConstructorHelpers::FClassFinder<UMyBuyMenuWidget> BP_BuyMenu(TEXT("WidgetBlueprint'/Game/Blueprints/UIs/BPMyBuyMenuWidget.BPMyBuyMenuWidget_C'"));

	if (BP_Widget.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("BuyMenu is loaded"));
		BuyMenu->SetWidgetClass(BP_BuyMenu.Class);
	}

	BuyMenu->SetWidgetSpace(EWidgetSpace::Screen);
}

void AMyInGameHUD::UpdateAmmo(const int32 CurrentAmmoCount, const int32 RemainingAmmoCount) const
{
	const auto& Widget = Cast<UMyInGameWidget>(Widgets->GetUserWidgetObject());

	if (Widget)
	{
		Widget->UpdateAmmo(CurrentAmmoCount, RemainingAmmoCount);
	}
}

UMyInGameWidget* AMyInGameHUD::GetInGameWidget() const
{
	return Cast<UMyInGameWidget>(Widgets->GetUserWidgetObject());
}

bool AMyInGameHUD::IsBuyMenuOpened() const
{
	const auto& BuyMenuWidget = Cast<UMyBuyMenuWidget>(BuyMenu->GetUserWidgetObject());

	if (BuyMenuWidget)
	{
		return BuyMenuWidget->IsOpened();
	}

	return false;
}

void AMyInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	const auto& Controller = GetOwningPlayerController();
	EnableInput(Controller);
	const auto& Character = Cast<AMyCharacter>(GetOwningPawn());

	const auto& PlayerState = Cast<AMyPlayerState>(Controller->PlayerState);
	const auto& Widget = Cast<UMyInGameWidget>(Widgets->GetUserWidgetObject());

	const auto& BuyMenuWidget = Cast<UMyBuyMenuWidget>(BuyMenu->GetUserWidgetObject());
	const auto& GameState = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));

	if (!IsValid(GameState))
	{
		UE_LOG(LogTemp, Error, TEXT("GameState is not valid"));
	}

	if (Widget)
	{
		Widget->AddToViewport();
	}

	if (BuyMenuWidget)
	{
		BuyMenuWidget->Populate();
		InputComponent->BindAction(TEXT("BuyMenu"), IE_Pressed, BuyMenuWidget, &UMyBuyMenuWidget::Toggle);
		GameState->BindOnBuyChanged(BuyMenuWidget, &UMyBuyMenuWidget::BuyTimeEnded);
	}
}

void AMyInGameHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AMyInGameHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Widgets->InitWidget();
	BuyMenu->InitWidget();
}
