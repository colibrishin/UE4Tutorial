// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyInGameHUD.h"

#include "MyCharacter.h"
#include "MyInGameWidget.h"

#include "Components/WidgetComponent.h"

AMyInGameHUD::AMyInGameHUD()
{
	Widgets = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widgets"));

	SetRootComponent(Widgets);
	Widgets->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> BP_Widget(TEXT("WidgetBlueprint'/Game/Blueprints/UIs/BPMyInGameUIWidget.BPMyInGameUIWidget_C'"));

	if (BP_Widget.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget is loaded"));
		Widgets->SetWidgetClass(BP_Widget.Class);
	}
}

void AMyInGameHUD::BindBomb(AMyC4* Bomb) const
{
	const auto& Widget = Cast<UMyInGameWidget>(Widgets->GetUserWidgetObject());

	if (Widget)
	{
		Widget->BindBomb(Bomb);
	}
}

void AMyInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	const auto& Widget = Cast<UMyInGameWidget>(Widgets->GetUserWidgetObject());

	if (Widget)
	{
		Widget->AddToViewport();
		Widget->BindPlayer(Cast<AMyCharacter>(GetOwningPawn()));
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
}
