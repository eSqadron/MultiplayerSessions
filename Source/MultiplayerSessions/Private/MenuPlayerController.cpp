// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuPlayerController.h"
#include "MenuWidget.h"

AMenuPlayerController::AMenuPlayerController()
{
	MainMenuWidgetClass = UMenuWidget::StaticClass();
}

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr == MainMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuWidgetClass is not set in AMenuPlayerController!"));
		return;
	}

	MainMenuWidget = CreateWidget<UMenuWidget>(GetWorld(), MainMenuWidgetClass);
	if (nullptr == MainMenuWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuWidget could not be created in AMenuPlayerController!"));
		return;
	}

	MainMenuWidget->MenuSetup(5, TEXT("FreeForAll"), TEXT("/Game/Lobby/LobbyMap"));
	MainMenuWidget->AddToViewport();

	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());
}
