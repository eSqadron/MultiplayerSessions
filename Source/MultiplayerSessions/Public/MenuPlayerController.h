// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerPlayerControllerBase.h"
#include "MenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API AMenuPlayerController : public AMultiplayerPlayerControllerBase
{
	GENERATED_BODY()

public:
	AMenuPlayerController();

	void BeginPlay() override;

public: // Properties

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMenuWidget> MainMenuWidgetClass;

private:
	class UMenuWidget* MainMenuWidget = nullptr;
	
};
