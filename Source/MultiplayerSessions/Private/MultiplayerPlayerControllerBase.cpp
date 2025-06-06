// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerPlayerControllerBase.h"
#include "GameFramework/PlayerState.h"

FString AMultiplayerPlayerControllerBase::GetUniqueNetID() const
{
	APlayerState* state = GetPlayerState<APlayerState>();

	if (nullptr == state)
	{
		UE_LOG(LogTemp, Error, TEXT("state is nullptr in GetUniqueNetID"));
		return FString();
	}

	const FUniqueNetIdRepl netID = state->GetUniqueId();
	if (netID.IsValid())
	{
		return netID->ToString();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't get unique netID!"));
		return state->GetPlayerName();
	}
}

FString AMultiplayerPlayerControllerBase::GetPlayerName() const
{
	APlayerState* state = GetPlayerState<APlayerState>();
	if (nullptr == state)
	{
		UE_LOG(LogTemp, Error, TEXT("state is nullptr in GetPlayerName"));
		return FString();
	}

	if (state->GetPlayerName().Len() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerName is empty in GetPlayerName"));
		return FString::Printf(TEXT("Player %d"), PlayerIndex);
	}

	return state->GetPlayerName();
}
