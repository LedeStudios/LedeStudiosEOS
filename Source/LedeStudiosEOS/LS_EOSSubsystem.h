// Copyright Lede Studios. All Rights Reserved.

#pragma once

#include "Containers/Set.h"
#include "CoreMinimal.h"

#include "GameFramework/OnlineReplStructs.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlinePartyInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineStats.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Achievements/AchievementsTypes.h"

#include "Beacon/LSEOS_Beacon.h"
#include "Friends/FriendsTypes.h"
#include "Leaderboards/LeaderboardsTypes.h"
#include "Lobbies/LobbiesTypes.h"
#include "Parties/PartiesTypes.h"
#include "Session/SessionTypes.h"
#include "Stats/StatsTypes.h"
#include "TitleFile/TitleFileTypes.h"
#include "UserCloud/UserCloudTypes.h"
#include "VoiceChat/VoiceChatServices.h"
#include "VoiceChat/VoiceTypes.h"
#include "LS_EOSSubsystem.generated.h"

#define SESSION_NAME	FName(TEXT("MyLocalSessionName"))

// =======================================================================
//  Achievements Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_QueryAchievementsComplete, bool, bWasSuccessful, const TArray<FLSEOS_Achievement>&, Achievements);

// =======================================================================
//  Auth Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_LoginComplete, bool, WasSuccessful, FString, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_LogoutComplete, bool, WasSuccessful);

// =======================================================================
//  Avatar Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_GetAvatarComplete, bool, bWasSuccessful, const TSoftObjectPtr<class UTexture>&, ResultTexture);

// =======================================================================
//  Friends Delegates
// =======================================================================
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLSEOS_FriendsChanged);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_ReadFriendsComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_ReadUsersComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_FriendOperationComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_SetFriendAliasComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_DeleteFriendAliasComplete, bool, WasSuccessful);

// =======================================================================
//  Leaderboards Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_QueryLeaderboardsComplete, bool, bWasSuccessful, const TArray<FLSEOS_LeaderboardEntry>&, LeaderboardEntries);

// =======================================================================
//  Lobbies Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_OnCreateOrConnectLobbyComplete, const FLSEOS_CreateOrConnectLobbyResult&, LobbyResult, bool, bWasSucessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_OnSearchLobbyComplete, const FLSEOS_LobbySearchResult&, SearchResult, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_OnUpdateLobbyComplete, const FLSEOS_UpdateLobbyResult&, SearchResult, bool, bWasSuccessful);

// =======================================================================
//  Parties Delegates
// =======================================================================
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLSEOS_InvitationsChanged);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_CreatePartyComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_JoinPartyComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_LeavePartyComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_KickMemberComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_InviteFriendComplete, bool, WasSuccessful);

// =======================================================================
//  Presence Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_UpdatePresenceComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_QueryPresenceComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLSEOS_PresenceUpdated, FString, UserId, FString, Status);

// =======================================================================
//  Session Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_FindSessionsComplete, bool, WasSuccessful, const TArray<ULSEOS_SessionSearchResult*>&, Results);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_CreateSessionComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_StartSessionComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_EndSessionComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_DestroySessionComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_JoinSessionComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLSEOS_PartyLeaderWasFollowedToSession, FName, SessionName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLSEOS_SessionJoinedMapTravelRequired, FName, SessionName);

// =======================================================================
//  Stats Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_QueryStatsComplete, bool, bWasSuccessful, const TArray<FLSEOS_Stat>&, QueriedStats);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_IngestStatComplete, bool, bWasSuccessful);

// =======================================================================
//  TitleFile Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_ReadTitleFileComplete, bool, bWasSuccessful, const FString&, FileName);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FLSEOS_EnumerateTitleFilesComplete, bool, bWasSuccessful, const TArray<FString>&, FileNames, const FString&, Error);

// =======================================================================
//  UserCloud Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_WriteUserFileComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLSEOS_WriteUserFileProgress,float,Percent,const FString &,FileName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLSEOS_WriteUserFileCancelled, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_ReadUserFileAsStringComplete,bool,WasSuccessful,const FString &,FileData);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_ReadUserFileAsSaveGameComplete,bool,WasSuccessful,class USaveGame *,FileData);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_EnumerateUserFilesComplete,bool,WasSuccessful,const TArray<FLSEOS_CloudFileData> &,FileData);

// =======================================================================
//  UserInfo Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_QueryUserInfoComplete, FString, Result);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_QueryUserIdMappingComplete, FString, Result);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLSEOS_QueryExternalIdMappingsComplete, FString, Result);

// =======================================================================
//  VoiceChar Delegates
// =======================================================================
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_OnVoiceChatLoginComplete,const FString &,PlayerName,const FLSEOS_VoiceChatResult &,VoiceChatLogin);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_OnVoiceChatLogoutComplete,const FString &,PlayerName,const FLSEOS_VoiceChatResult &,VoiceChatLogin);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_OnVoiceChatChannelJoinedComplete,const FString &,ChannelName,const FLSEOS_VoiceChatResult &,VoiceChatResult);

/**
 *
 */
UCLASS()
class LEDESTUDIOSEOS_API ULS_EOSSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
// =======================================================================
//  Unreal Engine Overrides
// =======================================================================
public:
	virtual void PostInitProperties() override;

// =======================================================================
//  Utilities
// =======================================================================
public:
	UFUNCTION(BlueprintCallable)
	FString UniqueNetIdToString(const FUniqueNetIdRepl& UniqueNetId);
	
// =======================================================================
//  Achievements
// =======================================================================
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void QueryAchievements(const UObject* WorldContextObject, FLSEOS_QueryAchievementsComplete OnDone);

private:
	void HandleQueryAchievementDescriptions(const FUniqueNetId& UserId, const bool bWasSuccessful, const UObject* WorldContextObject, FLSEOS_QueryAchievementsComplete OnDone);
	void HandleQueryAchievements(const FUniqueNetId& UserId, const bool bWasSuccessful, const UObject* WorldContextObject, FLSEOS_QueryAchievementsComplete OnDone);

// =======================================================================
//  Auth
// =======================================================================
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	bool IsLoggedIn(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	FString GetLoggedInUsername(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	FString GetLoggedInProductUserId(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	FString GetLoggedInEpicAccountId(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	FString GetLoggedInAuthAttribute(const UObject* WorldContextObject, const FString& InAuthAttrName);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	FUniqueNetIdRepl GetLoggedInUserId(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	bool CanLinkToEpicGamesAccount(const UObject* WorldContextObject);

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void StartLogin(const UObject* WorldContextObject, int32 LocalUserNum, FLSEOS_LoginComplete OnDone);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void StartLogout(const UObject *WorldContextObject, FLSEOS_LogoutComplete OnDone);

private:
	FDelegateHandle LoginDelegateHandle;
	FDelegateHandle LogoutDelegateHandle;
	
private:
	void HandleLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId &UserId,
		const FString &Error, const UObject* WorldContextObject, FLSEOS_LoginComplete OnDone);
	void HandleLogoutComplete(int32 LocalUserNum, bool bWasSuccessful, const UObject* WorldContextObject,
		FLSEOS_LogoutComplete OnDone);

// =======================================================================
//  Avatar
// =======================================================================
public:
	void GetAvatar(const int32 LocalUserId, const FUniqueNetIdRepl &TargetUserId, const UTexture* DefaultTexture, FLSEOS_GetAvatarComplete OnDone);

private:
	void HandleGetAvatarComplete(bool bWasSuccessful, TSoftObjectPtr<UTexture> ResultTexture, FLSEOS_GetAvatarComplete OnDone);

// =======================================================================
//  Friends
// =======================================================================
public:
	UPROPERTY(BlueprintAssignable)
	FLSEOS_FriendsChanged OnFriendsChanged;
	
private:
	void OnFriendsChange() const;

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	TArray<ULSEOS_Friend*> GetFriends(const UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void StartReadFriends(const UObject *WorldContextObject, FLSEOS_ReadFriendsComplete OnDone);

private:
    void HandleReadFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName,
    	const FString& ErrorStr, const UObject* WorldContextObject, FLSEOS_ReadFriendsComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    TArray<FLSEOS_SimpleUser> GetBlockedUsers(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartReadBlockedUsers(const UObject *WorldContextObject, FLSEOS_ReadUsersComplete OnDone);

private:
    FDelegateHandle QueryBlockedPlayersHandle;
    void HandleReadBlockedUsersComplete(const FUniqueNetId& UserId, const bool bWasSuccessful, const FString& Error,
    	const UObject* WorldContextObject, FLSEOS_ReadUsersComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    TArray<FLSEOS_SimpleUser> GetRecentPlayers(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartReadRecentPlayers(const UObject *WorldContextObject, FLSEOS_ReadUsersComplete OnDone);

private:
    FDelegateHandle QueryRecentPlayersHandle;
    void HandleReadRecentPlayersComplete(const FUniqueNetId& UserId, const FString& Namespace, const bool bWasSuccessful,
    	const FString& Error, const UObject* WorldContextObject, FLSEOS_ReadUsersComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void AcceptFriendInvite(const UObject* WorldContextObject, const FString& TargetUserId, FLSEOS_FriendOperationComplete OnDone);

private:
    void HandleAcceptFriendInviteComplete(const int32 LocalUserNum, const bool bWasSuccessful, const FUniqueNetId& FriendId,
    	const FString& ListName, const FString& ErrorStr, const UObject* WorldContextObject, FLSEOS_FriendOperationComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void BlockPlayer(const UObject* WorldContextObject, const FString& TargetUserId, FLSEOS_FriendOperationComplete OnDone);

private:
    FDelegateHandle BlockPlayerCompleteHandle;
    void HandleBlockPlayerComplete(const int32 LocalUserNum, const bool bWasSuccessful, const FUniqueNetId& UniqueID,const FString& ListName,
    	const FString& ErrorStr, const UObject* WorldContextObject, FLSEOS_FriendOperationComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void DeleteFriend(const UObject *WorldContextObject, const FString &TargetUserId, FLSEOS_FriendOperationComplete OnDone);

private:
    FDelegateHandle DeleteFriendCompleteHandle;
    void HandleDeleteFriendComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId &FriendId,
    	const FString &ListName, const FString &ErrorStr, const UObject *WorldContextObject, FLSEOS_FriendOperationComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void RejectFriendInvite(const UObject *WorldContextObject, const FString &TargetUserId, FLSEOS_FriendOperationComplete OnDone);

private:
    FDelegateHandle RejectFriendInviteCompleteHandle;
    void HandleRejectFriendInvite(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId &FriendId,
    	const FString &ListName, const FString &ErrorStr, const UObject *WorldContextObject, FLSEOS_FriendOperationComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void SendFriendInvite(const UObject *WorldContextObject, const FString &TargetUserId, FLSEOS_FriendOperationComplete OnDone);

private:
    void HandleSendFriendInviteComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId &FriendId,
    	const FString &ListName, const FString &ErrorStr, const UObject *WorldContextObject, FLSEOS_FriendOperationComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void UnblockPlayer(const UObject *WorldContextObject, const FString &TargetUserId, FLSEOS_FriendOperationComplete OnDone);

private:
    FDelegateHandle UnblockPlayerCompleteHandle;
    void HandleUnblockPlayerComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId &UniqueID,
    	const FString &ListName, const FString &ErrorStr, const UObject *WorldContextObject, FLSEOS_FriendOperationComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartSetFriendAlias(const UObject *WorldContextObject, FUniqueNetIdRepl UserId, const FString &Alias, FLSEOS_SetFriendAliasComplete OnDone);

private:
    void HandleSetFriendAliasComplete(int32 LocalUserNum, const FUniqueNetId &FriendId, const FString &ListName,
    	const FOnlineError &Error, const UObject *WorldContextObject, FLSEOS_SetFriendAliasComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartDeleteFriendAlias(const UObject *WorldContextObject, FUniqueNetIdRepl UserId, FLSEOS_DeleteFriendAliasComplete OnDone);

private:
    void HandleDeleteFriendAliasComplete(int32 LocalUserNum, const FUniqueNetId &FriendId, const FString &ListName,
    	const FOnlineError &Error, const UObject *WorldContextObject, FLSEOS_DeleteFriendAliasComplete OnDone);

// =======================================================================
//  ExternalUI
// =======================================================================
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void ShowFriendsUI(const UObject *WorldContextObject) const;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void ShowInviteUI(const UObject *WorldContextObject, FName SessionName) const;

// =======================================================================
//  Leaderboards
// =======================================================================
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void QueryFriendsLeaderboards(const UObject *WorldContextObject, FLSEOS_QueryLeaderboardsComplete OnDone);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void QueryGlobalLeaderboards(const UObject *WorldContextObject, const FString &LeaderboardId, FLSEOS_QueryLeaderboardsComplete OnDone);

private:
	FDelegateHandle QueryLeaderboardsDelegateHandle;
	void HandleLeaderboardResult(const bool bWasSuccessful, FOnlineLeaderboardReadRef LeaderboardRef,
		const UObject *WorldContextObject, FLSEOS_QueryLeaderboardsComplete OnDone);

// =======================================================================
//  Lobbies
// =======================================================================
public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void CreateLobby(const int64 &ExampleAttributeData, FLSEOS_OnCreateOrConnectLobbyComplete OnDone);
	
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void ConnectLobby(ULSEOS_LobbyId *LobbyId, FLSEOS_OnCreateOrConnectLobbyComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void DisconnectLobby(ULSEOS_LobbyId *LobbyId, FLSEOS_OnCreateOrConnectLobbyComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void SearchLobby(FLSEOS_OnSearchLobbyComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void UpdateLobby(ULSEOS_LobbyId *LobbyId, const int64 &ExampleAttributeData, FLSEOS_OnUpdateLobbyComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    bool GetMemberCount(ULSEOS_LobbyId *LobbyId, int32 &OutMemberCount);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    bool GetMemberUserId(ULSEOS_LobbyId *LobbyId, const int32 &MemberIndex, FString &OutMemberId);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	ULSEOS_PartyInvite* ConvertLobbyIdToPartyJoinInfo(ULSEOS_LobbyId *LobbyId);

private:
	void HandleCreateLobbyComplete(const FOnlineError &Error, const FUniqueNetId &UserId,
		const TSharedPtr<FOnlineLobby> &Lobby, FLSEOS_OnCreateOrConnectLobbyComplete OnDone);

	void HandleConnectLobbyComplete(const FOnlineError &Error, const FUniqueNetId &UserId,
		const TSharedPtr<FOnlineLobby> &Lobby, FLSEOS_OnCreateOrConnectLobbyComplete OnDone);

	void HandleDisconnectLobbyComplete(const FOnlineError &Error, const FUniqueNetId &UserId,
		FLSEOS_OnCreateOrConnectLobbyComplete OnDone);

    void HandleSearchLobbyComplete(const FOnlineError &Error,const FUniqueNetId &UserId,
    	const TArray<TSharedRef<const FOnlineLobbyId>> &LobbyIds,FLSEOS_OnSearchLobbyComplete OnDone);

    void HandleUpdateLobbyComplete(const FOnlineError &Error,const FUniqueNetId &UserId,
    	FLSEOS_OnUpdateLobbyComplete OnDone);

// =======================================================================
//  Multiplayer
// =======================================================================
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "WorldContextObject"))
	void SeamlessTravel(const UObject *WorldContextObject, const FString& URL);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "WorldContextObject"))
	void NonSeamlessTravel(const UObject *WorldContextObject, const FString& URL);

	UFUNCTION(BlueprintCallable)
	void BeginRecordingReplay(AGameModeBase *GameMode);

// =======================================================================
//  Parties
// =======================================================================
private:
    TSet<FString> SessionFollowConsumedIds;

    void OnPartyDataReceived(const FUniqueNetId &LocalUserId, const FOnlinePartyId &PartyId,
    	const FName &Namespace, const FOnlinePartyData &PartyData);
	
    void OnPartyInvitesChanged(const FUniqueNetId &LocalUserId) const;

public:
    UPROPERTY(BlueprintAssignable)
    FLSEOS_InvitationsChanged OnInvitationsChanged;

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    TArray<ULSEOS_PartyInvite*> GetInvitations(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    TArray<ULSEOS_PartyId*> GetJoinedParties(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void RequestPartyMembersFollowLeader(const UObject *WorldContextObject, FName SessionName);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    TArray<ULSEOS_PartyMemberId*> GetPartyMembers(const UObject *WorldContextObject, ULSEOS_PartyId* PartyId);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    bool IsPartyLeader(const UObject *WorldContextObject, ULSEOS_PartyId *PartyId);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartCreateParty(const UObject *WorldContextObject, int PartyTypeId, FLSEOS_CreatePartyComplete OnDone);

private:
    void HandleCreatePartyComplete(const FUniqueNetId &LocalUserId, const TSharedPtr<const FOnlinePartyId> &PartyId,
    	const ECreatePartyCompletionResult Result, const UObject *WorldContextObject, FLSEOS_CreatePartyComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartJoinParty(const UObject *WorldContextObject, ULSEOS_PartyInvite *Invite, FLSEOS_JoinPartyComplete OnDone);

private:
    void HandleJoinPartyComplete(const FUniqueNetId &LocalUserId,const FOnlinePartyId &PartyId,
    	const EJoinPartyCompletionResult Result, const int32 NotApprovedReason,const UObject *WorldContextObject,
    	FLSEOS_JoinPartyComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartLeaveParty(const UObject *WorldContextObject, ULSEOS_PartyId *PartyId, FLSEOS_LeavePartyComplete OnDone);

private:
    void HandleLeavePartyComplete(const FUniqueNetId &LocalUserId, const FOnlinePartyId &PartyId,
    	const ELeavePartyCompletionResult Result, const UObject *WorldContextObject, FLSEOS_LeavePartyComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartKickMember(const UObject *WorldContextObject, ULSEOS_PartyId *PartyId, ULSEOS_PartyMemberId *MemberId,
    	FLSEOS_KickMemberComplete OnDone);

private:
    void HandleKickMemberComplete(const FUniqueNetId &LocalUserId, const FOnlinePartyId &PartyId,
    	const FUniqueNetId &MemberId, const EKickMemberCompletionResult Result, const UObject *WorldContextObject,
    	FLSEOS_KickMemberComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartInviteFriend(const UObject *WorldContextObject, ULSEOS_PartyId *PartyId, ULSEOS_Friend *Friend,
    	FLSEOS_InviteFriendComplete OnDone);

private:
    void HandleInviteFriendComplete(const FUniqueNetId &LocalUserId,const FOnlinePartyId &PartyId,
    	const FUniqueNetId &RecipientId, const ESendPartyInvitationCompletionResult Result,
    	const UObject *WorldContextObject, FLSEOS_InviteFriendComplete OnDone);

// =======================================================================
//  Presence
// =======================================================================
private:
	void OnPresenceReceived(const FUniqueNetId &UserId, const TSharedRef<FOnlineUserPresence> &Presence) const;

public:
	UPROPERTY(BlueprintAssignable)
	FLSEOS_PresenceUpdated PresenceUpdated;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void StartUpdatePresence(const UObject *WorldContextObject, const FString &NewPresenceString,
		FLSEOS_UpdatePresenceComplete OnDone);

private:
	void HandleUpdatePresenceComplete(const FUniqueNetId &UserId, const bool bWasSuccessful,
		const UObject *WorldContextObject,FLSEOS_UpdatePresenceComplete OnDone);

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void QueryPresence(const UObject *WorldContextObject, FLSEOS_QueryPresenceComplete OnDone);

private:
	void HandleQueryPresenceComplete(const FUniqueNetId &UserId, const bool bWasSuccessful);

// =======================================================================
//  Session
// =======================================================================
private:
    void OnFindSessionForLeaderFollow(int32 LocalUserNum, bool bWasSuccessful,
    	const FOnlineSessionSearchResult &SearchResult, FString SessionFollowString);
	
    FName JoinSessionForLeaderSessionName;
    FDelegateHandle JoinSessionForLeaderFollowDelegateHandle;
    void OnJoinSessionForLeaderFollow(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);

    void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum,
    	TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult &InviteResult);
	
    void OnSessionJoinedViaOverlay(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult) const;
	
    FName JoinSessionForInviteSessionName;
    FDelegateHandle JoinSessionForInviteDelegateHandle;
    void OnJoinSessionForInviteFollow(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);

public:
    UPROPERTY(BlueprintAssignable)
    FLSEOS_PartyLeaderWasFollowedToSession PartyLeaderWasFollowedToSession;

    UPROPERTY(BlueprintAssignable)
    FLSEOS_SessionJoinedMapTravelRequired SessionJoinedMapTravelRequired;

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FString GetCurrentSessionId(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    TArray<FString> GetPlayersInSession(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void RegisterExistingPlayers(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FUniqueNetIdRepl RegisterPlayer(APlayerController *InPlayerController);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void UnregisterPlayer(APlayerController *InPlayerController, FUniqueNetIdRepl UniqueNetIdRepl);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void SendBeaconPingToSearchResult(const UObject *WorldContextObject, ULSEOS_SessionSearchResult *SearchResult,
    	FLSEOS_BeaconPingComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FString GetResolvedConnectString(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FString GetResolvedConnectStringForSession(const UObject *WorldContextObject, FName SessionName);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartCreateSession(const UObject *WorldContextObject, int32 Slots, FLSEOS_CreateSessionComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    bool InviteFriendToSession(const UObject *WorldContextObject, FName SessionName, FUniqueNetIdRepl FriendId);

private:
    FDelegateHandle CreateSessionDelegateHandle;
    void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful, const UObject *WorldContextObject,
    	FLSEOS_CreateSessionComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartFindSessions(const UObject *WorldContextObject, FLSEOS_FindSessionsComplete OnDone);

private:
    FDelegateHandle FindSessionsDelegateHandle;
    void HandleFindSessionsComplete(bool bWasSuccessful, const UObject *WorldContextObject,
    	FLSEOS_FindSessionsComplete OnDone,TSharedRef<FOnlineSessionSearch> Search);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartStartSession(const UObject *WorldContextObject, FName SessionName, FLSEOS_StartSessionComplete OnDone);

private:
    FDelegateHandle StartSessionDelegateHandle;
    void HandleStartSessionComplete(FName SessionName, bool bWasSuccessful, const UObject *WorldContextObject,
    	FLSEOS_StartSessionComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartEndSession(const UObject *WorldContextObject, FName SessionName, FLSEOS_EndSessionComplete OnDone);

private:
    FDelegateHandle EndSessionDelegateHandle;
    void HandleEndSessionComplete(FName SessionName, bool bWasSuccessful, const UObject *WorldContextObject,
    	FLSEOS_EndSessionComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartDestroySession(const UObject *WorldContextObject, FName SessionName, FLSEOS_DestroySessionComplete OnDone);

private:
    FDelegateHandle DestroySessionDelegateHandle;
    void HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful, const UObject *WorldContextObject,
    	FLSEOS_DestroySessionComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartJoinSession(const UObject *WorldContextObject, ULSEOS_SessionSearchResult *SearchResult,
    	FLSEOS_JoinSessionComplete OnDone);

private:
    FDelegateHandle JoinSessionDelegateHandle;
    void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult,
    	const UObject *WorldContextObject, FLSEOS_JoinSessionComplete OnDone);

// =======================================================================
//  Stats
// =======================================================================
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void QueryStats(const UObject *WorldContextObject, FLSEOS_QueryStatsComplete OnDone);

private:
	void HandleQueryStats(const FOnlineError &ResultState, const TArray<TSharedRef<const FOnlineStatsUserStats>> &UsersStatsResult,
		const UObject *WorldContextObject, FLSEOS_QueryStatsComplete OnDone);

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void IngestStat(const UObject *WorldContextObject, FString StatName, int32 IngestAmount,
		FLSEOS_IngestStatComplete OnDone);

private:
	void HandleIngestStat(const FOnlineError &ResultState, const UObject *WorldContextObject, FLSEOS_IngestStatComplete OnDone);

// =======================================================================
//  TitleFile
// =======================================================================
public:
	UFUNCTION(BlueprintCallable, Category = "Title")
	void ReadTitleFile(const FString &FileName, FLSEOS_ReadTitleFileComplete OnDone);

	UFUNCTION(BlueprintCallable, Category = "Title")
	void EnumerateTitleFiles(FLSEOS_EnumerateTitleFilesComplete OnDone);

	UFUNCTION(BlueprintCallable, Category = "Title")
	FString GetFileContents(const FString &FileName) const;

private:
	void HandleReadTitleFileComplete(
		bool WasSuccessful,
		const FString &FileName,
		FLSEOS_ReadTitleFileComplete OnDone);

	void HandleEnumerateTitleFilesComplete(
		bool WasSuccessful,
		const FString &Error,
		FLSEOS_EnumerateTitleFilesComplete OnDone) const;

private:
	static FString ReadFileDataAsString(const ULSEOS_FileData *FileData);

// =======================================================================
//  UserCloud
// =======================================================================
private:
    void WriteUserFile(const FString &FileName, TArray<uint8> &FileData, FLSEOS_WriteUserFileComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, Category = "User Cloud")
    void WriteUserFileFromString(const FString &FileName, const FString &FileData, FLSEOS_WriteUserFileComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "User Cloud")
    void WriteUserFileFromSaveGame(const FString &FileName, USaveGame *SaveGame, FLSEOS_WriteUserFileComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "User Cloud")
    void ReadUserFileAsString(const FString &FileName, FLSEOS_ReadUserFileAsStringComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "User Cloud")
    void ReadUserFileAsSaveGame(const FString &FileName, FLSEOS_ReadUserFileAsSaveGameComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "User Cloud")
    void EnumerateUserFiles(FLSEOS_EnumerateUserFilesComplete OnDone);

private:
    void HandleWriteUserFileComplete(bool WasSuccessful, const FUniqueNetId &UserId, const FString &FileName,
    	FLSEOS_WriteUserFileComplete OnDone);

    void HandleWriteUserFileCancelled(bool WasSuccessful, const FUniqueNetId &UserId, const FString &FileName) const;

    void HandleWriteUserFileProgress(int32 BytesWritten, const FUniqueNetId &UserId, const FString &FileName,
    	int32 TotalFileSize) const;

    void HandleReadUserFileComplete(bool WasSuccessful, const FUniqueNetId &UserId, const FString &FileName,
    	FLSEOS_ReadUserFileAsStringComplete OnDone) const;

    void HandleReadUserFileComplete(bool WasSuccessful, const FUniqueNetId &UserId, const FString &FileName,
    	FLSEOS_ReadUserFileAsSaveGameComplete OnDone) const;

    void HandleEnumerateUserFilesComplete(bool WasSuccessful, const FUniqueNetId &UserId,
    	FLSEOS_EnumerateUserFilesComplete OnDone) const;

public:
    UPROPERTY(BlueprintAssignable, Category = "User Cloud")
    FLSEOS_WriteUserFileProgress ExampleCPPSubsystemWriteUserFileProgress;

    UPROPERTY(BlueprintAssignable, Category = "User Cloud")
    FLSEOS_WriteUserFileCancelled ExampleCPPSubsystemWriteUserFileCancelled;

// =======================================================================
//  UserInfo
// =======================================================================
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void QueryUserInfo(const UObject *WorldContextObject, FString ProductUserIdInput,
		FLSEOS_QueryUserInfoComplete OnDone);

private:
	FDelegateHandle QueryUserInfoDelegateHandle;
	void HandleQueryUserInfoComplete(int32 LocalUserNum, bool bWasSuccessful,
		const TArray<TSharedRef<const FUniqueNetId>> &UserIds,const FString &ErrorStr,
		const UObject *WorldContextObject, FLSEOS_QueryUserInfoComplete OnDone);

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void QueryUserIdMapping(const UObject *WorldContextObject, FString DisplayNameInput,
		FLSEOS_QueryUserIdMappingComplete OnDone);

private:
	void HandleQueryUserMappingComplete(bool bWasSuccessful, const FUniqueNetId &UserId,
		const FString &DisplayNameOrEmail,const FUniqueNetId &FoundUserId, const FString &Error,
		const UObject *WorldContextObject, FLSEOS_QueryUserIdMappingComplete OnDone);

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void QueryExternalIdMappings(const UObject *WorldContextObject, FString PlatformType, FString ExternalIdsStr,
		FLSEOS_QueryExternalIdMappingsComplete OnDone);

private:
	void HandleQueryExternalIdMappings(bool bWasSuccessful, const FUniqueNetId &UserId,
		const FExternalIdQueryOptions &QueryOptions,const TArray<FString> &ExternalIds, const FString &Error,
		const UObject *WorldContextObject, FLSEOS_QueryExternalIdMappingsComplete OnDone);

// =======================================================================
//  VoiceChat
// =======================================================================
public:
	UFUNCTION(BlueprintCallable, Category = "Voice")
	void LoginToVoice(AVoiceChatServices *VoiceChatService, FLSEOS_OnVoiceChatLoginComplete OnDone);

	UFUNCTION(BlueprintCallable, Category = "Voice")
	void LogoutOfVoice(AVoiceChatServices *VoiceChatService, FLSEOS_OnVoiceChatLogoutComplete OnDone);

	UFUNCTION(BlueprintCallable, Category = "Voice")
	void JoinVoiceChannel(AVoiceChatServices *VoiceChatService, const FString &ChannelName,
		const FString &ChannelCredentials,const ELSEOS_VoiceChatChannelType ChannelType,
		const FLSEOS_VoiceChatChannel3dProperties Channel3dProperties, FLSEOS_OnVoiceChatChannelJoinedComplete OnDone);

	UFUNCTION(BlueprintCallable, Category = "Voice")
	bool IsConnected() const;

	UFUNCTION(BlueprintCallable, Category = "Voice")
	bool IsConnecting() const;

private:
	void HandleVoiceChatLoginComplete(const FString &PlayerName, const FVoiceChatResult &Result,
		FLSEOS_OnVoiceChatLoginComplete OnDone);

	void HandleVoiceChatLogoutComplete(const FString &PlayerName, const FVoiceChatResult &Result,
		AVoiceChatServices *VoiceChatService, FLSEOS_OnVoiceChatLogoutComplete OnDone);

	void HandleVoiceChatJoinChannelComplete(const FString &ChannelName, const FVoiceChatResult &Result, 
		FLSEOS_OnVoiceChatChannelJoinedComplete OnDone);
};


