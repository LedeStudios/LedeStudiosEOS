// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void ULS_EOSSubsystem::OnFriendsChange() const
{
	this->OnFriendsChanged.Broadcast();
}

TArray<ULSEOS_Friend*> ULS_EOSSubsystem::GetFriends(const UObject* WorldContextObject)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return TArray<ULSEOS_Friend*>();
	}
	
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	TArray<TSharedRef<FOnlineFriend>> FriendsRaw;
	if (!Friends->GetFriendsList(0, TEXT(""), FriendsRaw))
	{
		return TArray<ULSEOS_Friend*>();
	}

	TArray<ULSEOS_Friend*> Results;
	for (const TSharedRef<FOnlineFriend>& Friend : FriendsRaw)
	{
		ULSEOS_Friend *FriendResult = NewObject<ULSEOS_Friend>(this);
		FriendResult->Friend = Friend;
		Results.Add(FriendResult);
	}
	return Results;
}

void ULS_EOSSubsystem::StartReadFriends(const UObject* WorldContextObject, FLSEOS_ReadFriendsComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	if (!Friends->ReadFriendsList(0,TEXT(""),
		FOnReadFriendsListComplete::CreateUObject(
			this,&ULS_EOSSubsystem::HandleReadFriendsComplete,WorldContextObject,OnDone)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleReadFriendsComplete(int32 LocalUserNum, const bool bWasSuccessful, const FString& ListName,
	const FString& ErrorStr, const UObject* WorldContextObject, FLSEOS_ReadFriendsComplete OnDone)
{
	OnDone.ExecuteIfBound(bWasSuccessful);
}

TArray<FLSEOS_SimpleUser> ULS_EOSSubsystem::GetBlockedUsers(const UObject* WorldContextObject)
{
	TArray<FLSEOS_SimpleUser> Results;

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return Results;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	TArray<TSharedRef<FOnlineBlockedPlayer>> BlockedUsers;
	if (!Friends->GetBlockedPlayers(*Identity->GetUniquePlayerId(0), BlockedUsers))
	{
		return Results;
	}

	for (const TSharedRef<FOnlineBlockedPlayer>& BlockedUser : BlockedUsers)
	{
		FLSEOS_SimpleUser SimpleUser;
		SimpleUser.UserId = BlockedUser->GetUserId()->ToString();
		SimpleUser.DisplayName = BlockedUser->GetDisplayName();
		Results.Add(SimpleUser);
	}
	
	return Results;
}

void ULS_EOSSubsystem::StartReadBlockedUsers(const UObject* WorldContextObject, FLSEOS_ReadUsersComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();
	
	this->QueryBlockedPlayersHandle = Friends->AddOnQueryBlockedPlayersCompleteDelegate_Handle(
		FOnQueryBlockedPlayersCompleteDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleReadBlockedUsersComplete,WorldContextObject,OnDone));

	if (!Friends->QueryBlockedPlayers(*Identity->GetUniquePlayerId(0)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleReadBlockedUsersComplete(const FUniqueNetId& UserId, const bool bWasSuccessful,
	const FString& Error, const UObject* WorldContextObject, FLSEOS_ReadUsersComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	Friends->ClearOnQueryBlockedPlayersCompleteDelegate_Handle(this->QueryBlockedPlayersHandle);
	
	OnDone.ExecuteIfBound(bWasSuccessful);
}

TArray<FLSEOS_SimpleUser> ULS_EOSSubsystem::GetRecentPlayers(const UObject* WorldContextObject)
{
	TArray<FLSEOS_SimpleUser> Results;

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return Results;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	TArray<TSharedRef<FOnlineRecentPlayer>> RecentPlayers;
	if (!Friends->GetRecentPlayers(*Identity->GetUniquePlayerId(0), TEXT(""), RecentPlayers))
	{
		return Results;
	}
	
	for (const TSharedRef<FOnlineRecentPlayer>& RecentPlayer : RecentPlayers)
	{
		FLSEOS_SimpleUser SimpleUser;
		SimpleUser.UserId = RecentPlayer->GetUserId()->ToString();
		SimpleUser.DisplayName = RecentPlayer->GetDisplayName();
		Results.Add(SimpleUser);
	}
	return Results;
}

void ULS_EOSSubsystem::StartReadRecentPlayers(const UObject* WorldContextObject, FLSEOS_ReadUsersComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	this->QueryRecentPlayersHandle = Friends->AddOnQueryRecentPlayersCompleteDelegate_Handle(
		FOnQueryRecentPlayersCompleteDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleReadRecentPlayersComplete,WorldContextObject,OnDone));

	if (!Friends->QueryRecentPlayers(*Identity->GetUniquePlayerId(0), TEXT("")))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleReadRecentPlayersComplete(const FUniqueNetId& UserId, const FString& Namespace,
	const bool bWasSuccessful, const FString& Error, const UObject* WorldContextObject, FLSEOS_ReadUsersComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	Friends->ClearOnQueryRecentPlayersCompleteDelegate_Handle(this->QueryRecentPlayersHandle);

	OnDone.ExecuteIfBound(bWasSuccessful);
}

void ULS_EOSSubsystem::AcceptFriendInvite(const UObject* WorldContextObject, const FString& TargetUserId,
	FLSEOS_FriendOperationComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	if (!Friends->AcceptInvite(0, *Identity->CreateUniquePlayerId(TargetUserId),TEXT(""),
			FOnAcceptInviteComplete::CreateUObject(
				this, &ULS_EOSSubsystem::HandleAcceptFriendInviteComplete,WorldContextObject,OnDone)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleAcceptFriendInviteComplete(const int32 LocalUserNum, const bool bWasSuccessful,
	const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorStr, const UObject* WorldContextObject,
	FLSEOS_FriendOperationComplete OnDone)
{
	OnDone.ExecuteIfBound(bWasSuccessful);
}

void ULS_EOSSubsystem::BlockPlayer(const UObject* WorldContextObject, const FString& TargetUserId,
	FLSEOS_FriendOperationComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	this->BlockPlayerCompleteHandle = Friends->AddOnBlockedPlayerCompleteDelegate_Handle(0,
		FOnBlockedPlayerCompleteDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleBlockPlayerComplete,WorldContextObject,OnDone));

	if (!Friends->BlockPlayer(0, *Identity->CreateUniquePlayerId(TargetUserId)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleBlockPlayerComplete(const int32 LocalUserNum, const bool bWasSuccessful,
	const FUniqueNetId& UniqueID, const FString& ListName, const FString& ErrorStr, const UObject* WorldContextObject,
	FLSEOS_FriendOperationComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	Friends->ClearOnBlockedPlayerCompleteDelegate_Handle(0, this->BlockPlayerCompleteHandle);

	OnDone.ExecuteIfBound(bWasSuccessful);
}

void ULS_EOSSubsystem::DeleteFriend(const UObject* WorldContextObject, const FString& TargetUserId,
	FLSEOS_FriendOperationComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	this->DeleteFriendCompleteHandle = Friends->AddOnDeleteFriendCompleteDelegate_Handle(0,
		FOnDeleteFriendCompleteDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleDeleteFriendComplete,WorldContextObject,OnDone));

	if (!Friends->DeleteFriend(0, *Identity->CreateUniquePlayerId(TargetUserId), TEXT("")))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleDeleteFriendComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& FriendId,
	const FString& ListName, const FString& ErrorStr, const UObject* WorldContextObject,
	FLSEOS_FriendOperationComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	Friends->ClearOnDeleteFriendCompleteDelegate_Handle(0, this->DeleteFriendCompleteHandle);

	OnDone.ExecuteIfBound(bWasSuccessful);
}

void ULS_EOSSubsystem::RejectFriendInvite(const UObject* WorldContextObject, const FString& TargetUserId,
	FLSEOS_FriendOperationComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	this->RejectFriendInviteCompleteHandle = Friends->AddOnRejectInviteCompleteDelegate_Handle(0,
		FOnRejectInviteCompleteDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleRejectFriendInvite,WorldContextObject,OnDone));

	if (!Friends->RejectInvite(0, *Identity->CreateUniquePlayerId(TargetUserId), TEXT("")))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleRejectFriendInvite(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& FriendId,
	const FString& ListName, const FString& ErrorStr, const UObject* WorldContextObject,
	FLSEOS_FriendOperationComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	Friends->ClearOnRejectInviteCompleteDelegate_Handle(0, this->RejectFriendInviteCompleteHandle);

	OnDone.ExecuteIfBound(bWasSuccessful);
}

void ULS_EOSSubsystem::SendFriendInvite(const UObject* WorldContextObject, const FString& TargetUserId,
	FLSEOS_FriendOperationComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	if (!Friends->SendInvite(0,*Identity->CreateUniquePlayerId(TargetUserId),TEXT(""),
		FOnSendInviteComplete::CreateUObject(
			this, &ULS_EOSSubsystem::HandleSendFriendInviteComplete,WorldContextObject,OnDone)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleSendFriendInviteComplete(int32 LocalUserNum, bool bWasSuccessful,
	const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorStr, const UObject* WorldContextObject,
	FLSEOS_FriendOperationComplete OnDone)
{
	OnDone.ExecuteIfBound(bWasSuccessful);
}

void ULS_EOSSubsystem::UnblockPlayer(const UObject* WorldContextObject, const FString& TargetUserId,
	FLSEOS_FriendOperationComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	this->UnblockPlayerCompleteHandle = Friends->AddOnUnblockedPlayerCompleteDelegate_Handle(0,
		FOnUnblockedPlayerCompleteDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleUnblockPlayerComplete,WorldContextObject,OnDone));

	if (!Friends->UnblockPlayer(0, *Identity->CreateUniquePlayerId(TargetUserId)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleUnblockPlayerComplete(int32 LocalUserNum, bool bWasSuccessful,
	const FUniqueNetId& UniqueID, const FString& ListName, const FString& ErrorStr, const UObject* WorldContextObject,
	FLSEOS_FriendOperationComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	Friends->ClearOnUnblockedPlayerCompleteDelegate_Handle(0, this->UnblockPlayerCompleteHandle);

	OnDone.ExecuteIfBound(bWasSuccessful);
}

void ULS_EOSSubsystem::StartSetFriendAlias(const UObject* WorldContextObject, FUniqueNetIdRepl UserId,
	const FString& Alias, FLSEOS_SetFriendAliasComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	Friends->SetFriendAlias(0, *UserId.GetUniqueNetId(), TEXT(""), Alias,
		FOnSetFriendAliasComplete::CreateUObject(
			this,&ULS_EOSSubsystem::HandleSetFriendAliasComplete,WorldContextObject,OnDone));
}

void ULS_EOSSubsystem::HandleSetFriendAliasComplete(int32 LocalUserNum, const FUniqueNetId& FriendId,
	const FString& ListName, const FOnlineError& Error, const UObject* WorldContextObject,
	FLSEOS_SetFriendAliasComplete OnDone)
{
	OnDone.ExecuteIfBound(Error.bSucceeded);
}

void ULS_EOSSubsystem::StartDeleteFriendAlias(const UObject* WorldContextObject, FUniqueNetIdRepl UserId,
	FLSEOS_DeleteFriendAliasComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	Friends->DeleteFriendAlias(0, *UserId.GetUniqueNetId(), TEXT(""),
		FOnDeleteFriendAliasComplete::CreateUObject(
			this, &ULS_EOSSubsystem::HandleDeleteFriendAliasComplete,WorldContextObject,OnDone));
}

void ULS_EOSSubsystem::HandleDeleteFriendAliasComplete(int32 LocalUserNum, const FUniqueNetId& FriendId,
	const FString& ListName, const FOnlineError& Error, const UObject* WorldContextObject,
	FLSEOS_DeleteFriendAliasComplete OnDone)
{
	OnDone.ExecuteIfBound(Error.bSucceeded);
}