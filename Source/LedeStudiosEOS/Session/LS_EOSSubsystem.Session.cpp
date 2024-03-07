// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Config/LSEOS_ConfigLibrary.h"

#include "Engine/LocalPlayer.h"
#include "Engine/NetConnection.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlinePresenceInterface.h"

void ULS_EOSSubsystem::OnFindSessionForLeaderFollow(int32 LocalUserNum, const bool bWasSuccessful,
                                                    const FOnlineSessionSearchResult& SearchResult, FString SessionFollowString)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	check(Subsystem != nullptr);
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	if (bWasSuccessful)
	{
		this->JoinSessionForLeaderSessionName = FName(SessionFollowString);
		this->JoinSessionForLeaderFollowDelegateHandle =
			Session->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionComplete::FDelegate::CreateUObject(
				this,
				&ULS_EOSSubsystem::OnJoinSessionForLeaderFollow));
		if (!Session->JoinSession(0, FName(SessionFollowString), SearchResult))
		{
			UE_LOG(LogTemp, Error, TEXT("Could not call JoinSession after following party leader!"));
		}
	}
}

void ULS_EOSSubsystem::OnJoinSessionForLeaderFollow(const FName SessionName, const EOnJoinSessionCompleteResult::Type JoinResult)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	check(Subsystem != nullptr);
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	if (JoinResult == EOnJoinSessionCompleteResult::Success && SessionName.IsEqual(JoinSessionForLeaderSessionName))
	{
		this->PartyLeaderWasFollowedToSession.Broadcast(SessionName);
		Session->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionForLeaderFollowDelegateHandle);
	}
}

void ULS_EOSSubsystem::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum,
	TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
{
	check(bWasSuccessful); // Should always be true in EOS subsystem.

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	check(Subsystem != nullptr);
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	this->JoinSessionForInviteSessionName = FName(*InviteResult.GetSessionIdStr());
	this->JoinSessionForInviteDelegateHandle = Session->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionComplete::FDelegate::CreateUObject(this, &ULS_EOSSubsystem::OnJoinSessionForInviteFollow));
	if (!Session->JoinSession(0, FName(*InviteResult.GetSessionIdStr()), InviteResult))
	{
		UE_LOG(LogTemp, Error, TEXT("Could not call JoinSession after accepting invite!"));
	}
}

void ULS_EOSSubsystem::OnSessionJoinedViaOverlay(const FName SessionName, const EOnJoinSessionCompleteResult::Type JoinResult) const
{
	if (JoinResult == EOnJoinSessionCompleteResult::Success &&
	SessionName.ToString().StartsWith(TEXT("JoinViaOverlay_")))
	{
		this->SessionJoinedMapTravelRequired.Broadcast(SessionName);
	}
}

void ULS_EOSSubsystem::OnJoinSessionForInviteFollow(const FName SessionName, const EOnJoinSessionCompleteResult::Type JoinResult)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	check(Subsystem != nullptr);
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	if (JoinResult == EOnJoinSessionCompleteResult::Success && SessionName.IsEqual(JoinSessionForInviteSessionName))
	{
		this->SessionJoinedMapTravelRequired.Broadcast(SessionName);
		Session->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionForInviteDelegateHandle);
	}
}

FString ULS_EOSSubsystem::GetCurrentSessionId(const UObject* WorldContextObject)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return TEXT("");
	}

	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Session == nullptr)
	{
		return TEXT("");
	}

	const FNamedOnlineSession* NamedSession = Session->GetNamedSession(SESSION_NAME);
	if (NamedSession == nullptr || !NamedSession->SessionInfo.IsValid() || !NamedSession->SessionInfo->IsValid())
	{
		return TEXT("");
	}

	return NamedSession->GetSessionIdStr();
}

TArray<FString> ULS_EOSSubsystem::GetPlayersInSession(const UObject* WorldContextObject)
{
	TArray<FString> Results;

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return Results;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Session == nullptr)
	{
		return Results;
	}

	FNamedOnlineSession* NamedSession = Session->GetNamedSession(SESSION_NAME);
	if (NamedSession == nullptr || !NamedSession->SessionInfo.IsValid() || !NamedSession->SessionInfo->IsValid())
	{
		return Results;
	}

	for (TSharedRef<const FUniqueNetId> PlayerId : NamedSession->RegisteredPlayers)
	{
		if (NamedSession->LocalOwnerId != nullptr && *PlayerId == *NamedSession->LocalOwnerId)
		{
			Results.Add(FString::Printf(TEXT("(host): %s"), *PlayerId->ToString()));
		}
		else
		{
			Results.Add(FString::Printf(TEXT("(client): %s"), *PlayerId->ToString()));
		}
	}

	return Results;
}

void ULS_EOSSubsystem::RegisterExistingPlayers(const UObject* WorldContextObject)
{
	for (auto It = WorldContextObject->GetWorld()->GetPlayerControllerIterator(); It; --It)
	{
		this->RegisterPlayer(It->Get());
	}
}

FUniqueNetIdRepl ULS_EOSSubsystem::RegisterPlayer(APlayerController* InPlayerController)
{
	check(IsValid(InPlayerController));

	if (InPlayerController->GetNetDriverName().IsEqual(NAME_DemoNetDriver))
	{
		return FUniqueNetIdRepl();
	}

	FUniqueNetIdRepl UniqueNetIdRepl;
	if (InPlayerController->IsLocalPlayerController())
	{
		const ULocalPlayer* LocalPlayer = InPlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
		}
		else
		{
			const UNetConnection *RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
			check(IsValid(RemoteNetConnection));
			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}
	}
	else
	{
		const UNetConnection *RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
		check(IsValid(RemoteNetConnection));
		UniqueNetIdRepl = RemoteNetConnection->PlayerId;
	}

	const TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
	if (!UniqueNetId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("No unique net ID assocated with connection, can not register player"));
		return FUniqueNetIdRepl();
	}

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(InPlayerController->GetWorld());
	check(Subsystem != nullptr);
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	check(Session != nullptr);

	verify(Session->RegisterPlayer(SESSION_NAME, *UniqueNetId, false));
	return UniqueNetIdRepl;
}

void ULS_EOSSubsystem::UnregisterPlayer(APlayerController* InPlayerController, FUniqueNetIdRepl UniqueNetIdRepl)
{
	check(IsValid(InPlayerController));

	const TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
	if (!UniqueNetId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("No unique net ID assocated with connection, can not unregister player"));
		return;
	}

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(InPlayerController->GetWorld());
	check(Subsystem != nullptr);
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	check(Session != nullptr);

	verify(Session->UnregisterPlayer(SESSION_NAME, *UniqueNetId));
}

void ULS_EOSSubsystem::SendBeaconPingToSearchResult(const UObject* WorldContextObject,
	ULSEOS_SessionSearchResult* SearchResult, FLSEOS_BeaconPingComplete OnDone)
{
	if (!ULSEOS_ConfigLibrary::GetAreBeaconsSupported(WorldContextObject))
	{
		OnDone.ExecuteIfBound(false, TEXT("Beacons are not supported on the legacy networking stack"));
		return;
	}

	if (!IsValid(SearchResult))
	{
		OnDone.ExecuteIfBound(false, TEXT("Search result is not valid"));
		return;
	}

	IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TEXT("No online subsystem"));
		return;
	}
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	// The port name is set to FName(TEXT("12345")) as per the comment in DemoBeacon.cpp.
	// Allow dedicated servers to specify an override for the beacon port as needed.
	FString BeaconPortString = TEXT("12345");
	FString OverrideBeaconPort;
	if (SearchResult->Result.Session.SessionSettings.Get("OverrideBeaconPort", OverrideBeaconPort))
	{
		BeaconPortString = OverrideBeaconPort;
	}

	FString ConnectInfo;
	if (!Session->GetResolvedConnectString(SearchResult->Result, FName(*BeaconPortString), ConnectInfo))
	{
		OnDone.ExecuteIfBound(false, TEXT("Connect info not found"));
		return;
	}

	if (auto Beacon = WorldContextObject->GetWorld()->SpawnActor<AOnlineBeacon>(ALSEOS_BeaconClient::StaticClass()))
	{
		Cast<ALSEOS_BeaconClient>(Beacon)->OnComplete = OnDone;
		FURL OldURL;
		FURL URL(&OldURL, *ConnectInfo, ETravelType::TRAVEL_Absolute);
		Cast<ALSEOS_BeaconClient>(Beacon)->InitClient(URL);
	}
}

FString ULS_EOSSubsystem::GetResolvedConnectString(const UObject* WorldContextObject)
{
	return GetResolvedConnectStringForSession(WorldContextObject, SESSION_NAME);
}

FString ULS_EOSSubsystem::GetResolvedConnectStringForSession(const UObject* WorldContextObject, const FName SessionName)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return TEXT("");
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	FString ConnectInfo;
	if (Session->GetResolvedConnectString(SessionName, ConnectInfo))
	{
		return ConnectInfo;
	}

	return TEXT("");
}


bool ULS_EOSSubsystem::InviteFriendToSession(const UObject* WorldContextObject, const FName SessionName,
	const FUniqueNetIdRepl FriendId)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return false;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	return Session->SendSessionInviteToFriend(0, SessionName, *FriendId.GetUniqueNetId());
}

void ULS_EOSSubsystem::StartCreateSession(const UObject* WorldContextObject, int32 Slots,
	FLSEOS_CreateSessionComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	// note: This example code only supports running one CreateSession process at once; if you want to support
	// multiple in flight you probably need to make a latent blueprint node.
	if (this->CreateSessionDelegateHandle.IsValid())
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	const TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
	SessionSettings->NumPublicConnections = Slots;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	bool IsDedicated = this->GetWorld()->GetNetMode() == NM_DedicatedServer;
	SessionSettings->Settings.Add(FName(TEXT("IsDedicatedServer")),
		FOnlineSessionSetting(IsDedicated, EOnlineDataAdvertisementType::ViaOnlineService));
	
	//TODO: Change Default Values
	SessionSettings->Settings.Add(
		FName(TEXT("SessionSetting")),
		FOnlineSessionSetting(FString(TEXT("SettingValue")), EOnlineDataAdvertisementType::ViaOnlineService));
	
	this->CreateSessionDelegateHandle =
		Session->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionComplete::FDelegate::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleCreateSessionComplete,
			WorldContextObject,
			OnDone));
	
	if (!Session->CreateSession(0, SESSION_NAME, *SessionSettings))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleCreateSessionComplete(FName SessionName, const bool bWasSuccessful,
	const UObject* WorldContextObject, FLSEOS_CreateSessionComplete OnDone)
{
	OnDone.ExecuteIfBound(bWasSuccessful);

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Session == nullptr)
	{
		return;
	}

	Session->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);
	this->CreateSessionDelegateHandle.Reset();
}

void ULS_EOSSubsystem::StartFindSessions(const UObject* WorldContextObject, FLSEOS_FindSessionsComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<ULSEOS_SessionSearchResult *>());
		return;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	// note: This example code only supports running one FindSessions process at once; if you want to support
	// multiple in flight you probably need to make a latent blueprint node.
	if (this->FindSessionsDelegateHandle.IsValid())
	{
		OnDone.ExecuteIfBound(false, TArray<ULSEOS_SessionSearchResult *>());
		return;
	}

	TSharedRef<FOnlineSessionSearch> Search = MakeShared<FOnlineSessionSearch>();
	
	// Remove the default search parameters that FOnlineSessionSearch sets up.
	Search->MaxSearchResults = 100;
	Search->QuerySettings.SearchParams.Empty();

	//TODO: Change Default Values
	Search->QuerySettings.Set(
		FName(TEXT("SessionSetting")),
		FString(TEXT("SettingValue")),
		EOnlineComparisonOp::Equals);

	this->FindSessionsDelegateHandle =
		Session->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsComplete::FDelegate::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleFindSessionsComplete,
			WorldContextObject,
			OnDone,
			Search));
	if (!Session->FindSessions(0, Search))
	{
		OnDone.ExecuteIfBound(false, TArray<ULSEOS_SessionSearchResult *>());
	}
}

void ULS_EOSSubsystem::HandleFindSessionsComplete(bool bWasSuccessful, const UObject* WorldContextObject,
	FLSEOS_FindSessionsComplete OnDone, TSharedRef<FOnlineSessionSearch> Search)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	check(Subsystem != nullptr);
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	check(Session != nullptr);

	TArray<ULSEOS_SessionSearchResult *> Results;
	if (bWasSuccessful)
	{
		for (auto RawResult : Search->SearchResults)
		{
			if (RawResult.IsValid())
			{
				auto Result = NewObject<ULSEOS_SessionSearchResult>(this);
				Result->Result = RawResult;
				Result->SessionId = RawResult.GetSessionIdStr();
				Result->OpenSlots = RawResult.Session.NumOpenPublicConnections;
				Result->TotalSlots = RawResult.Session.SessionSettings.NumPublicConnections;
				const auto DedicatedServerAttr = RawResult.Session.SessionSettings.Settings.Find("IsDedicatedServer");
				if (DedicatedServerAttr != nullptr)
				{
					DedicatedServerAttr->Data.GetValue(Result->bIsDedicatedServer);
				}
				else
				{
					Result->bIsDedicatedServer = false;
				}
				Session->GetResolvedConnectString(RawResult, NAME_GamePort, Result->ConnectionString);
				Results.Add(Result);
			}
		}
	}

	OnDone.ExecuteIfBound(bWasSuccessful, Results);

	Session->ClearOnFindSessionsCompleteDelegate_Handle(this->FindSessionsDelegateHandle);
	this->FindSessionsDelegateHandle.Reset();
}

void ULS_EOSSubsystem::StartStartSession(const UObject* WorldContextObject, FName SessionName,
	FLSEOS_StartSessionComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	// note: This example code only supports running one StartSession process at once; if you want to support
	// multiple in flight you probably need to make a latent blueprint node.
	if (this->StartSessionDelegateHandle.IsValid())
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	this->StartSessionDelegateHandle =
		Session->AddOnStartSessionCompleteDelegate_Handle(FOnStartSessionComplete::FDelegate::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleStartSessionComplete,
			WorldContextObject,
			OnDone));
	if (!Session->StartSession(SessionName))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleStartSessionComplete(FName SessionName, bool bWasSuccessful,
	const UObject* WorldContextObject, FLSEOS_StartSessionComplete OnDone)
{
	OnDone.ExecuteIfBound(bWasSuccessful);

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Session == nullptr)
	{
		return;
	}

	Session->ClearOnStartSessionCompleteDelegate_Handle(this->StartSessionDelegateHandle);
	this->StartSessionDelegateHandle.Reset();
}

void ULS_EOSSubsystem::StartEndSession(const UObject* WorldContextObject, FName SessionName,
	FLSEOS_EndSessionComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	// note: This example code only supports running one EndSession process at once; if you want to support
	// multiple in flight you probably need to make a latent blueprint node.
	if (this->EndSessionDelegateHandle.IsValid())
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	this->EndSessionDelegateHandle =
		Session->AddOnEndSessionCompleteDelegate_Handle(FOnEndSessionComplete::FDelegate::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleEndSessionComplete,
			WorldContextObject,
			OnDone));
	if (!Session->EndSession(SessionName))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleEndSessionComplete(FName SessionName, bool bWasSuccessful,
	const UObject* WorldContextObject, FLSEOS_EndSessionComplete OnDone)
{
	OnDone.ExecuteIfBound(bWasSuccessful);

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Session == nullptr)
	{
		return;
	}

	Session->ClearOnEndSessionCompleteDelegate_Handle(this->EndSessionDelegateHandle);
	this->EndSessionDelegateHandle.Reset();
}

void ULS_EOSSubsystem::StartDestroySession(const UObject* WorldContextObject, FName SessionName,
	FLSEOS_DestroySessionComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	// note: This example code only supports running one DestroySession process at once; if you want to support
	// multiple in flight you probably need to make a latent blueprint node.
	if (this->DestroySessionDelegateHandle.IsValid())
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	this->DestroySessionDelegateHandle =
		Session->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionComplete::FDelegate::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleDestroySessionComplete,
			WorldContextObject,
			OnDone));
	if (!Session->DestroySession(SessionName))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful,
	const UObject* WorldContextObject, FLSEOS_DestroySessionComplete OnDone)
{
	OnDone.ExecuteIfBound(bWasSuccessful);

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Session == nullptr)
	{
		return;
	}

	Session->ClearOnDestroySessionCompleteDelegate_Handle(this->DestroySessionDelegateHandle);
	this->DestroySessionDelegateHandle.Reset();
}

void ULS_EOSSubsystem::StartJoinSession(const UObject* WorldContextObject, ULSEOS_SessionSearchResult* SearchResult,
	FLSEOS_JoinSessionComplete OnDone)
{
	IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	// note: This example code only supports running one JoinSession process at once; if you want to support
	// multiple in flight you probably need to make a latent blueprint node.
	if (this->JoinSessionDelegateHandle.IsValid())
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	check(IsValid(SearchResult));

	this->JoinSessionDelegateHandle =
		Session->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionComplete::FDelegate::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleJoinSessionComplete,
			WorldContextObject,
			OnDone));
	
	if (!Session->JoinSession(0, SESSION_NAME, SearchResult->Result))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleJoinSessionComplete(const FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult,
	const UObject* WorldContextObject, FLSEOS_JoinSessionComplete OnDone)
{
	if (!SessionName.IsEqual(SESSION_NAME))
	{
		// Since we can conflict with the party leader following stuff... ugh the sessions API...
		return;
	}

	OnDone.ExecuteIfBound(
		JoinResult == EOnJoinSessionCompleteResult::Success ||
		JoinResult == EOnJoinSessionCompleteResult::AlreadyInSession);

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return;
	}
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Session == nullptr)
	{
		return;
	}

	Session->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionDelegateHandle);
	this->JoinSessionDelegateHandle.Reset();
}

