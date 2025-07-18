// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API AMultiplayerGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer);
	
};
