// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyInGameHUD.h"

#include "MyGameState.h"
#include "Components/WidgetComponent.h"
#include "MyProject/Widgets/MyBombProgressWidget.h"
#include "MyProject/Widgets/MyBuyMenuWidget.h"
#include "MyProject/Widgets/MyInGameStatWidget.h"
#include "MyProject/Widgets/MyInGameWidget.h"

AMyInGameHUD::AMyInGameHUD()
{
	Widgets = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widgets"));
	BuyMenu = CreateDefaultSubobject<UWidgetComponent>(TEXT("BuyMenu"));
	StatWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatWidget"));

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

	static ConstructorHelpers::FClassFinder<UMyInGameStatWidget> BP_StatWidget(TEXT("WidgetBlueprint'/Game/Blueprints/UIs/BPMyInGameStatWidget.BPMyInGameStatWidget_C'"));

	if (BP_StatWidget.Succeeded())
	{
		LOG_FUNC(LogTemp, Warning, "Stat is loaded");
		StatWidget->SetWidgetClass(BP_StatWidget.Class);
	}

	StatWidget->SetWidgetSpace(EWidgetSpace::Screen);
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
	// InitWidget are called inside of Super::BeginPlay by iterating through the component list.
	Super::BeginPlay();

	const auto& Controller = GetOwningPlayerController();
	EnableInput(Controller);
	const auto& Widget = Cast<UMyInGameWidget>(Widgets->GetUserWidgetObject());

	const auto& BuyMenuWidget = Cast<UMyBuyMenuWidget>(BuyMenu->GetUserWidgetObject());
	const auto& GameState = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));

	const auto& StatSubWidget = Cast<UMyInGameStatWidget>(StatWidget->GetUserWidgetObject());

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
		GameState->OnBuyChanged.AddUniqueDynamic(BuyMenuWidget, &UMyBuyMenuWidget::BuyTimeEnded);
	}

	if (StatSubWidget)
	{
		InputComponent->BindAction(TEXT("Score"), IE_Pressed, StatSubWidget, &UMyInGameStatWidget::Open);
		InputComponent->BindAction(TEXT("Score"), IE_Released, StatSubWidget, &UMyInGameStatWidget::Close);
	}
}

void AMyInGameHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AMyInGameHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
