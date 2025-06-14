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

	FString GetLocalRoleAsString() const;

	uint8 GetPlayerIndex() const { return PlayerIndex; }

	// TODO - replace it with something that actually gets the index from the client!
	void SetPlayerIndexOnServerSide(uint8 NewPlayerIndex) { PlayerIndex = NewPlayerIndex; }

	/*
	* This function should be called on the client side when setting a new player index.
	*/
	// this doesn't work - why?
	//UFUNCTION(Server, Reliable)
	//void ReplicateIndexToServer(uint8 NewPlayerIndex);

protected:
	uint8 PlayerIndex = 0;


	
};
