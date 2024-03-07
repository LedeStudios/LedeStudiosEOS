// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineLeaderboardInterface.h"

void ULS_EOSSubsystem::QueryFriendsLeaderboards(const UObject* WorldContextObject,
	FLSEOS_QueryLeaderboardsComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_LeaderboardEntry>());
		return;
	}
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineLeaderboardsPtr LeaderboardsInterface = Subsystem->GetLeaderboardsInterface();

	FOnlineLeaderboardReadRef ReadRef = MakeShared<FOnlineLeaderboardRead, ESPMode::ThreadSafe>();
	ReadRef->ColumnMetadata.Add(FColumnMetaData(FName(TEXT("TestScore")), EOnlineKeyValuePairDataType::Int32));
	
	// note: This example code only supports running one leaderboard process at once; if you want to support
	// multiple in flight you probably need to make a latent blueprint node.
	if (this->QueryLeaderboardsDelegateHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Already a leaderboard operation running."));
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_LeaderboardEntry>());
		return;
	}

	this->QueryLeaderboardsDelegateHandle = LeaderboardsInterface->AddOnLeaderboardReadCompleteDelegate_Handle(
		FOnLeaderboardReadComplete::FDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleLeaderboardResult,ReadRef,WorldContextObject,OnDone));
	
	if (!LeaderboardsInterface->ReadLeaderboardsForFriends(0, ReadRef))
	{
		LeaderboardsInterface->ClearOnLeaderboardReadCompleteDelegate_Handle(this->QueryLeaderboardsDelegateHandle);
		this->QueryLeaderboardsDelegateHandle.Reset();
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_LeaderboardEntry>());
	}
}

void ULS_EOSSubsystem::QueryGlobalLeaderboards(const UObject* WorldContextObject, const FString& LeaderboardId,
	FLSEOS_QueryLeaderboardsComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_LeaderboardEntry>());
		return;
	}
	
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineLeaderboardsPtr LeaderboardsInterface = Subsystem->GetLeaderboardsInterface();

	FOnlineLeaderboardReadRef ReadRef = MakeShared<FOnlineLeaderboardRead, ESPMode::ThreadSafe>();
	ReadRef->LeaderboardName = FName(LeaderboardId);

	// note: This example code only supports running one leaderboard process at once; if you want to support
	// multiple in flight you probably need to make a latent blueprint node.
	if (this->QueryLeaderboardsDelegateHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Already a leaderboard operation running."));
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_LeaderboardEntry>());
		return;
	}

	this->QueryLeaderboardsDelegateHandle = LeaderboardsInterface->AddOnLeaderboardReadCompleteDelegate_Handle(
		FOnLeaderboardReadComplete::FDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleLeaderboardResult, ReadRef, WorldContextObject, OnDone));
	
	if (!LeaderboardsInterface->ReadLeaderboardsAroundRank(0, 100, ReadRef))
	{
		LeaderboardsInterface->ClearOnLeaderboardReadCompleteDelegate_Handle(this->QueryLeaderboardsDelegateHandle);
		this->QueryLeaderboardsDelegateHandle.Reset();
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_LeaderboardEntry>());
	}
}

void ULS_EOSSubsystem::HandleLeaderboardResult(const bool bWasSuccessful, FOnlineLeaderboardReadRef LeaderboardRef,
	const UObject* WorldContextObject, FLSEOS_QueryLeaderboardsComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	check(Subsystem != nullptr);
	
	const IOnlineLeaderboardsPtr LeaderboardsInterface = Subsystem->GetLeaderboardsInterface();
	LeaderboardsInterface->ClearOnLeaderboardReadCompleteDelegate_Handle(this->QueryLeaderboardsDelegateHandle);
	this->QueryLeaderboardsDelegateHandle.Reset();

	if (!bWasSuccessful)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_LeaderboardEntry>());
		return;
	}

	TArray<FLSEOS_LeaderboardEntry> Results;
	for (const FOnlineStatsRow& Row : LeaderboardRef->Rows)
	{
		FLSEOS_LeaderboardEntry Entry;
		Entry.PlayerId = Row.PlayerId->ToString();
		Entry.PlayerNickname = Row.NickName;
		Entry.Rank = Row.Rank;
		if (Row.Columns.Contains(TEXT("Score")))
		{
			// For global leaderboards, column is always called "Score".
			Row.Columns[TEXT("Score")].GetValue(Entry.Score);
		}
		else
		{
			// For friend leaderboards, we requested the "TestScore" stat.
			Row.Columns[TEXT("TestScore")].GetValue(Entry.Score);
		}
		Results.Add(Entry);
	}

	OnDone.ExecuteIfBound(true, Results);
}


