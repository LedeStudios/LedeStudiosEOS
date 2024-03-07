// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void ULS_EOSSubsystem::QueryStats(const UObject* WorldContextObject, FLSEOS_QueryStatsComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_Stat>());
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineStatsPtr Stats = Subsystem->GetStatsInterface();

	TArray<TSharedRef<const FUniqueNetId>> Users;
	Users.Add(Identity->GetUniquePlayerId(0).ToSharedRef());

	TArray<FString> StatNames;
	StatNames.Add(TEXT("TestLatest"));
	StatNames.Add(TEXT("TestScore"));

	Stats->QueryStats(
		Identity->GetUniquePlayerId(0).ToSharedRef(),
		Users,
		StatNames,
		FOnlineStatsQueryUsersStatsComplete::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleQueryStats,
			WorldContextObject,
			OnDone));
}

void ULS_EOSSubsystem::HandleQueryStats(const FOnlineError& ResultState,
	const TArray<TSharedRef<const FOnlineStatsUserStats>>& UsersStatsResult, const UObject* WorldContextObject,
	FLSEOS_QueryStatsComplete OnDone)
{
	const TSharedRef<const FOnlineStatsUserStats> QueriedStats = UsersStatsResult[0];

	if (!ResultState.bSucceeded)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_Stat>());
		return;
	}

	TArray<FLSEOS_Stat> Results;
	for (const TTuple<FString, FVariantData>& Entry : QueriedStats->Stats)
	{
		FLSEOS_Stat Stat;
		Stat.Id = Entry.Key;
		Entry.Value.GetValue(Stat.CurrentValue);
		Results.Add(Stat);
	}

	OnDone.ExecuteIfBound(true, Results);
}

void ULS_EOSSubsystem::IngestStat(const UObject* WorldContextObject, FString StatName, int32 IngestAmount,
	FLSEOS_IngestStatComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineStatsPtr StatsInterface = Subsystem->GetStatsInterface();

	FOnlineStatsUserUpdatedStats Stat = FOnlineStatsUserUpdatedStats(Identity->GetUniquePlayerId(0).ToSharedRef());
	Stat.Stats.Add(StatName, FOnlineStatUpdate(IngestAmount, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));

	TArray<FOnlineStatsUserUpdatedStats> Stats;
	Stats.Add(Stat);

	StatsInterface->UpdateStats(
		Identity->GetUniquePlayerId(0).ToSharedRef(),
		Stats,
		FOnlineStatsUpdateStatsComplete::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleIngestStat,
			WorldContextObject,
			OnDone));
}

void ULS_EOSSubsystem::HandleIngestStat(const FOnlineError& ResultState, const UObject* WorldContextObject,
	FLSEOS_IngestStatComplete OnDone)
{
	OnDone.ExecuteIfBound(ResultState.bSucceeded);
}


