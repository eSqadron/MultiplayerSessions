// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerPlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API AMultiplayerPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public: // Methods:

	FString GetUniqueNetID() const;
	FString GetPlayerName() const;

protected:
	uint8 PlayerIndex = 0;
	
};
