// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameModeBase.h"
#include "GameFramework/GameStateBase.h"

void AMultiplayerGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
