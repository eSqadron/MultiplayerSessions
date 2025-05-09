// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"

void UMenuWidget::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (nullptr == World)
	{
		return;
	}
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (nullptr == PlayerController)
	{
		return;
	}

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetInputMode(InputModeData);

	UGameInstance* GameInstance = World->GetGameInstance();

	if (nullptr == GameInstance)
	{
		return;
	}

	MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (nullptr == MultiplayerSessionsSubsystem)
	{
		return;
	}

	MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMenuWidget::OnCreateSession);
	MultiplayerSessionsSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &UMenuWidget::OnFindSession);
	MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMenuWidget::OnJoinSession);
	MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMenuWidget::OnDestroySession);
	MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &UMenuWidget::OnStartSession);
}

bool UMenuWidget::Initialize()
{
	bool ret = Super::Initialize();

	if (nullptr == HostButton)
	{
		return false;
	}

	if (nullptr == JoinButton)
	{
		return false;
	}


	HostButton->OnClicked.AddDynamic(this, &UMenuWidget::HostButtonClicked);
	JoinButton->OnClicked.AddDynamic(this, &UMenuWidget::JoinButtonClicked);

	return ret;
}

void UMenuWidget::NativeDestruct()
{
	MenuTeardown();
	Super::NativeDestruct();
}

void UMenuWidget::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	JoinButton->SetIsEnabled(false);
	if (nullptr == MultiplayerSessionsSubsystem)
	{
		return;
	}

	MultiplayerSessionsSubsystem->CreateSession(4, TEXT("FreeForAll"));
}

void UMenuWidget::OnCreateSession(bool bWasSuccesfull)
{
	if (!bWasSuccesfull)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Red, TEXT("Failed To create session"));
		}

		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);

		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Green, TEXT("Session Succesfully created"));
	}

	UWorld* World = GetWorld();
	if (nullptr == World)
	{
		return;
	}
	World->ServerTravel(PathToLobby);
}

void UMenuWidget::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResult, bool bWasSuccesfull)
{
	if (nullptr == MultiplayerSessionsSubsystem)
	{
		return;
	}

	if (!bWasSuccesfull || SessionResult.Num() == 0)
	{
		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);
	}

	for (FOnlineSessionSearchResult result : SessionResult)
	{
		FString SettingValue;
		result.Session.SessionSettings.Get(FName("MatchType"), SettingValue);

		if (SettingValue != MatchType)
		{
			continue;
		}

		result.Session.SessionSettings.bUsesPresence = true;
		result.Session.SessionSettings.bUseLobbiesIfAvailable = true;
		MultiplayerSessionsSubsystem->JoinSession(result);
	}
}

void UMenuWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (nullptr == Subsystem)
	{
		return;
	}

	auto SessionInterface = Subsystem->GetSessionInterface();

	if(!SessionInterface.IsValid())
	{
		return;
	}

	FString address;
	if (!SessionInterface->GetResolvedConnectString(NAME_GameSession, address))
	{
		return;
	}

	APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();

	if (nullptr == playerController)
	{
		return;
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);
		return;
	}

	playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);

}

void UMenuWidget::OnDestroySession(bool bWasSuccesfull)
{
}

void UMenuWidget::OnStartSession(bool bWasSuccesfull)
{
	UWorld* World = GetWorld();
	if (nullptr == World)
	{
		return;
	}
	World->ServerTravel(TEXT("/Game/ThirdPerson/Maps/ThirdPersonMap?listen"));
}

void UMenuWidget::JoinButtonClicked()
{
	HostButton->SetIsEnabled(false);
	JoinButton->SetIsEnabled(false);
	if (nullptr == MultiplayerSessionsSubsystem)
	{
		return;
	}

	MultiplayerSessionsSubsystem->FindSessions(10000);
}

void UMenuWidget::MenuTeardown()
{
	RemoveFromParent();

	UWorld* World = GetWorld();

	if(nullptr == World)
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (nullptr == PlayerController)
	{
		return;
	}

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->SetShowMouseCursor(false);
}
