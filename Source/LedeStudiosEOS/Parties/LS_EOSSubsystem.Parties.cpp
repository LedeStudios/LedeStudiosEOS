// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

void ULS_EOSSubsystem::OnPartyDataReceived(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId,
                                           const FName& Namespace, const FOnlinePartyData& PartyData)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	check(Subsystem != nullptr);

	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePartyPtr PartySystem = Subsystem->GetPartyInterface();

	const FOnlinePartyConstPtr Party = PartySystem->GetParty(LocalUserId, PartyId);
	if (!Party.IsValid() || *Party->LeaderId == LocalUserId)
	{
		// Ignore this request because we're either not in the party (?) or
		// we're the leader.
		return;
	}

	FVariantData Data;
	if (PartyData.GetAttribute(TEXT("SessionFollowString"), Data))
	{
		if (Data.GetType() == EOnlineKeyValuePairDataType::String)
		{
			// This party has a SessionFollowString, parse it.
			FString SessionFollowString;
			Data.GetValue(SessionFollowString);
			TArray<FString> SessionFollowComponents;
			SessionFollowString.ParseIntoArray(SessionFollowComponents, TEXT("|"));
			const FString RequestId = SessionFollowComponents[0];
			const FString SessionId = SessionFollowComponents[1];

			if (this->SessionFollowConsumedIds.Contains(RequestId))
			{
				// We have already acted upon this before.
				return;
			}

			this->SessionFollowConsumedIds.Add(RequestId);

			// Try to find the session.
			Session->FindSessionById(
				*Identity->GetUniquePlayerId(0),
				*Session->CreateSessionIdFromString(SessionId),
				*Identity->GetUniquePlayerId(0),
				FOnSingleSessionResultCompleteDelegate::CreateUObject(
					this, &ULS_EOSSubsystem::OnFindSessionForLeaderFollow,SessionFollowString));
		}
	}
}

void ULS_EOSSubsystem::OnPartyInvitesChanged(const FUniqueNetId& LocalUserId) const
{
	this->OnInvitationsChanged.Broadcast();
}

TArray<ULSEOS_PartyInvite*> ULS_EOSSubsystem::GetInvitations(const UObject* WorldContextObject)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return TArray<ULSEOS_PartyInvite*>();
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

	TArray<IOnlinePartyJoinInfoConstRef> PendingInvites;
	if (!Party->GetPendingInvites(*Identity->GetUniquePlayerId(0).Get(), PendingInvites))
	{
		return TArray<ULSEOS_PartyInvite*>();
	}

	TArray<ULSEOS_PartyInvite*> Results;
	for (const IOnlinePartyJoinInfoConstRef Entry : PendingInvites)
	{
		ULSEOS_PartyInvite* Id = NewObject<ULSEOS_PartyInvite>(this);
		Id->PartyInvite = Entry;
		Results.Add(Id);
	}
	
	return Results;
}

TArray<ULSEOS_PartyId*> ULS_EOSSubsystem::GetJoinedParties(const UObject* WorldContextObject)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return TArray<ULSEOS_PartyId*>();
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

	TArray<TSharedRef<const FOnlinePartyId>> Result;
	if (!Party->GetJoinedParties(*Identity->GetUniquePlayerId(0).Get(), Result))
	{
		return TArray<ULSEOS_PartyId*>();
	}

	TArray<ULSEOS_PartyId*> ResultIds;
	for (const TSharedRef<const FOnlinePartyId>& Entry : Result)
	{
		ULSEOS_PartyId* Id = NewObject<ULSEOS_PartyId>(this);
		Id->PartyId = Entry;
		ResultIds.Add(Id);
	}
	return ResultIds;
}

void ULS_EOSSubsystem::RequestPartyMembersFollowLeader(const UObject* WorldContextObject, FName SessionName)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePartyPtr PartySystem = Subsystem->GetPartyInterface();
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	const FUniqueNetId* LocalUser = Identity->GetUniquePlayerId(0).Get();

	TArray<TSharedRef<const FOnlinePartyId>> Result;
	if (!PartySystem->GetJoinedParties(*LocalUser, Result))
	{
		return;
	}

	const FNamedOnlineSession* SessionData = Session->GetNamedSession(SessionName);
	const FString SessionFollowString = FString::Printf(TEXT("%d|%s"), FMath::Rand(), *SessionData->GetSessionIdStr());

	for (const TSharedRef<const FOnlinePartyId>& PartyId : Result)
	{
		FOnlinePartyConstPtr Party = PartySystem->GetParty(*LocalUser, *PartyId);
		if (Party.IsValid())
		{
			if (*Party->LeaderId == *LocalUser)
			{
				// We are the leader, so we can make other members follow.
				TSharedRef<FOnlinePartyData> PartyDataUpdate = MakeShared<FOnlinePartyData>();
				PartyDataUpdate->SetAttribute(TEXT("SessionFollowString"), SessionFollowString);
				if (!PartySystem->UpdatePartyData(*LocalUser, *PartyId, NAME_Default, *PartyDataUpdate))
				{
					UE_LOG(LogTemp,Error,TEXT("Could not request other members to follow because UpdatePartyData failed."));
				}
			}
		}
	}
}

TArray<ULSEOS_PartyMemberId*> ULS_EOSSubsystem::GetPartyMembers(const UObject* WorldContextObject,
	ULSEOS_PartyId* PartyId)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return TArray<ULSEOS_PartyMemberId*>();
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

	TArray<FOnlinePartyMemberConstRef> PartyMembers;
	if (!Party->GetPartyMembers(*Identity->GetUniquePlayerId(0).Get(), *PartyId->PartyId.Get(), PartyMembers))
	{
		return TArray<ULSEOS_PartyMemberId*>();
	}

	TArray<ULSEOS_PartyMemberId*> Results;
	for (const FOnlinePartyMemberConstRef& Entry : PartyMembers)
	{
		ULSEOS_PartyMemberId* Id = NewObject<ULSEOS_PartyMemberId>(this);
		Id->PartyMember = Entry;
		Results.Add(Id);
	}
	return Results;
}

bool ULS_EOSSubsystem::IsPartyLeader(const UObject* WorldContextObject, ULSEOS_PartyId* PartyId)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return false;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

	return Party->IsMemberLeader(*Identity->GetUniquePlayerId(0).Get(),
		*PartyId->PartyId.Get(),*Identity->GetUniquePlayerId(0).Get());
}

void ULS_EOSSubsystem::StartCreateParty(const UObject* WorldContextObject, const int PartyTypeId,
	FLSEOS_CreatePartyComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

	const TSharedRef<FPartyConfiguration> Config = MakeShared<FPartyConfiguration>();
	Config->bIsAcceptingMembers = true;
	Config->MaxMembers = 4;
	
	// You must set InvitePermissions to ::Anyone in order for synthetic parties to work.
	Config->InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;

	if (!Party->CreateParty(*Identity->GetUniquePlayerId(0).Get(), FOnlinePartyTypeId(PartyTypeId), *Config,
		FOnCreatePartyComplete::CreateUObject(
			this,&ULS_EOSSubsystem::HandleCreatePartyComplete,WorldContextObject,OnDone)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleCreatePartyComplete(const FUniqueNetId& LocalUserId,
	const TSharedPtr<const FOnlinePartyId>& PartyId, const ECreatePartyCompletionResult Result,
	const UObject* WorldContextObject, FLSEOS_CreatePartyComplete OnDone)
{
	OnDone.ExecuteIfBound(Result == ECreatePartyCompletionResult::Succeeded);
}

void ULS_EOSSubsystem::StartJoinParty(const UObject* WorldContextObject, ULSEOS_PartyInvite* Invite,
	FLSEOS_JoinPartyComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

	if (!Party->JoinParty(*Identity->GetUniquePlayerId(0).Get(),*Invite->PartyInvite,
		FOnJoinPartyComplete::CreateUObject(
			this, &ULS_EOSSubsystem::HandleJoinPartyComplete,WorldContextObject,OnDone)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleJoinPartyComplete(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId,
	const EJoinPartyCompletionResult Result, const int32 NotApprovedReason, const UObject* WorldContextObject,
	FLSEOS_JoinPartyComplete OnDone)
{
	OnDone.ExecuteIfBound(Result == EJoinPartyCompletionResult::Succeeded);
}

void ULS_EOSSubsystem::StartLeaveParty(const UObject* WorldContextObject, ULSEOS_PartyId* PartyId,
	FLSEOS_LeavePartyComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

	if (!Party->LeaveParty(*Identity->GetUniquePlayerId(0).Get(),*PartyId->PartyId,
		FOnLeavePartyComplete::CreateUObject(
			this, &ULS_EOSSubsystem::HandleLeavePartyComplete,WorldContextObject,OnDone)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleLeavePartyComplete(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId,
	const ELeavePartyCompletionResult Result, const UObject* WorldContextObject, FLSEOS_LeavePartyComplete OnDone)
{
	OnDone.ExecuteIfBound(Result == ELeavePartyCompletionResult::Succeeded);
}

void ULS_EOSSubsystem::StartKickMember(const UObject* WorldContextObject, ULSEOS_PartyId* PartyId,
	ULSEOS_PartyMemberId* MemberId, FLSEOS_KickMemberComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

	if (!Party->KickMember(*Identity->GetUniquePlayerId(0).Get(),
		*PartyId->PartyId,*MemberId->PartyMember->GetUserId(),
		FOnKickPartyMemberComplete::CreateUObject(
			this, &ULS_EOSSubsystem::HandleKickMemberComplete, WorldContextObject, OnDone)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleKickMemberComplete(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId,
	const FUniqueNetId& MemberId, const EKickMemberCompletionResult Result, const UObject* WorldContextObject,
	FLSEOS_KickMemberComplete OnDone)
{
	OnDone.ExecuteIfBound(Result == EKickMemberCompletionResult::Succeeded);
}

void ULS_EOSSubsystem::StartInviteFriend(const UObject* WorldContextObject, ULSEOS_PartyId* PartyId,
	ULSEOS_Friend* Friend, FLSEOS_InviteFriendComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

	const FPartyInvitationRecipient Recipient = FPartyInvitationRecipient(Friend->Friend->GetUserId());

	if (!Party->SendInvitation(*Identity->GetUniquePlayerId(0).Get(), *PartyId->PartyId, Recipient,
		FOnSendPartyInvitationComplete::CreateUObject(
			this, &ULS_EOSSubsystem::HandleInviteFriendComplete,WorldContextObject,OnDone)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleInviteFriendComplete(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId,
	const FUniqueNetId& RecipientId, const ESendPartyInvitationCompletionResult Result,
	const UObject* WorldContextObject, FLSEOS_InviteFriendComplete OnDone)
{
	OnDone.ExecuteIfBound(Result == ESendPartyInvitationCompletionResult::Succeeded);
}


