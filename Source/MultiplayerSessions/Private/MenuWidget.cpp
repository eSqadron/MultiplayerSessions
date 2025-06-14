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
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] World is null in MenuSetup"));
		return;
	}
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (nullptr == PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] PlayerController is null in MenuSetup"));
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
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] GameInstance is null in MenuSetup"));
		return;
	}

	MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (nullptr == MultiplayerSessionsSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] MultiplayerSessionsSubsystem is null in GameInstance"));
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
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] HostButton is null in Initialize"));
		return false;
	}

	if (nullptr == JoinButton)
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] JoinButton is null in Initialize"));
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
	UE_LOG(LogTemp, Display, TEXT("[MenuWidget] Host button clicked!"));
	HostButton->SetIsEnabled(false);
	JoinButton->SetIsEnabled(false);
	if (nullptr == MultiplayerSessionsSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] MultiplayerSessionsSubsystem is null in HostButtonClicked"));
		return;
	}

	MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
}

void UMenuWidget::OnCreateSession(bool bWasSuccesfull)
{
	if (!bWasSuccesfull)
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] Failed to create a session in OnCreateSession"));

		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);

		return;
	}

	UE_LOG(LogTemp, Display, TEXT("[MenuWidget] Succesfully created session"));

	UWorld* World = GetWorld();
	if (nullptr == World)
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] World is null in OnCreateSession"));
		return;
	}
	World->ServerTravel(PathToLobby);
}

void UMenuWidget::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResult, bool bWasSuccesfull)
{
	if (nullptr == MultiplayerSessionsSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] MultiplayerSessionsSubsystem is null in OnFindSession"));
		return;
	}

	if (!bWasSuccesfull || SessionResult.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] Failed to find a session in OnFindSession. bWasSuccesfull: %d SessionResult.Num(): %d"), bWasSuccesfull, SessionResult.Num());
		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("[MenuWidget] Succesfully found %d session in OnFindSession."), SessionResult.Num());

	for (FOnlineSessionSearchResult result : SessionResult)
	{
		FString SettingValue;
		result.Session.SessionSettings.Get(FName("MatchType"), SettingValue);

		if (SettingValue != MatchType)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MenuWidget] Skipping session of match type %s in OnFindSession."), *SettingValue);
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
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] OnlineSubsystem is null in OnJoinSession"));
		return;
	}

	auto SessionInterface = Subsystem->GetSessionInterface();

	if(!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] SessionInterface isn't valid in OnJoinSession"));
		return;
	}

	FString address;
	if (!SessionInterface->GetResolvedConnectString(NAME_GameSession, address))
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] Failed to get resolved connect string in OnJoinSession"));
		return;
	}

	APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();

	if (nullptr == playerController)
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] PlayerController is null in OnJoinSession"));
		return;
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] Failed to join session in OnJoinSession. Result: %d"), Result);
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
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] World null in OnStartSession"));
		return;
	}
	World->ServerTravel(TEXT("/Game/ThirdPerson/Maps/ThirdPersonMap?listen"));
}

void UMenuWidget::JoinButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("[MenuWidget] Join button clicked!"));
	HostButton->SetIsEnabled(false);
	JoinButton->SetIsEnabled(false);
	if (nullptr == MultiplayerSessionsSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuWidget] MultiplayerSessionsSubsystem is nullptr in JoinButtonClicked"));
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
