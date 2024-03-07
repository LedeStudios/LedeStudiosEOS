// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"


void ULS_EOSSubsystem::OnPresenceReceived(const FUniqueNetId& UserId, const TSharedRef<FOnlineUserPresence>& Presence) const
{
	PresenceUpdated.Broadcast(FUniqueNetIdRepl(UserId).ToString(), Presence->Status.ToDebugString());
}

void ULS_EOSSubsystem::StartUpdatePresence(const UObject* WorldContextObject, const FString& NewPresenceString,
	FLSEOS_UpdatePresenceComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlinePresencePtr Presence = Subsystem->GetPresenceInterface();

	FOnlineUserPresenceStatus Status;
	Status.State = EOnlinePresenceState::Online;
	Status.StatusStr = NewPresenceString;

	Presence->SetPresence(
		*Identity->GetUniquePlayerId(0).Get(),
		Status,
		IOnlinePresence::FOnPresenceTaskCompleteDelegate::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleUpdatePresenceComplete,
			WorldContextObject,
			OnDone));
}

void ULS_EOSSubsystem::HandleUpdatePresenceComplete(const FUniqueNetId& UserId, const bool bWasSuccessful,
	const UObject* WorldContextObject, FLSEOS_UpdatePresenceComplete OnDone)
{
	OnDone.ExecuteIfBound(bWasSuccessful);
}

void ULS_EOSSubsystem::QueryPresence(const UObject* WorldContextObject, FLSEOS_QueryPresenceComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	check(Subsystem != nullptr);
	const IOnlinePresencePtr PresenceInterface = Subsystem->GetPresenceInterface();
	check(PresenceInterface != nullptr);
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	check(Identity != nullptr);

	PresenceInterface->QueryPresence(
		*Identity->GetUniquePlayerId(0),
		IOnlinePresence::FOnPresenceTaskCompleteDelegate::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleQueryPresenceComplete));
}

void ULS_EOSSubsystem::HandleQueryPresenceComplete(const FUniqueNetId& UserId, const bool bWasSuccessful)
{
	UE_LOG(LogTemp, Verbose, TEXT("Query presence result: %s"), bWasSuccessful ? TEXT("true") : TEXT("false"));
}