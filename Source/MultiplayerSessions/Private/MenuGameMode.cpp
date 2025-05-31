// Fill out your copyright notice in the Description page of Project Settings.
#include "MenuGameMode.h"
#include "MenuPlayerController.h"

AMenuGameMode::AMenuGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = AMenuPlayerController::StaticClass();
}
