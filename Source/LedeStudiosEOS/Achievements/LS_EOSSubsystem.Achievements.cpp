// Copyright Lede Studios. All Rights Reserved.

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineAchievementsInterface.h"

void ULS_EOSSubsystem::QueryAchievements(const UObject* WorldContextObject, FLSEOS_QueryAchievementsComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_Achievement>());
		return;
	}
	
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineAchievementsPtr AchievementsInterface = Subsystem->GetAchievementsInterface();
	
	AchievementsInterface->QueryAchievementDescriptions(
		*Identity->GetUniquePlayerId(0), FOnQueryAchievementsCompleteDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleQueryAchievementDescriptions,WorldContextObject,OnDone));
}

void ULS_EOSSubsystem::HandleQueryAchievementDescriptions(const FUniqueNetId& UserId, const bool bWasSuccessful,
	const UObject* WorldContextObject, FLSEOS_QueryAchievementsComplete OnDone)
{
	if (!bWasSuccessful)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_Achievement>());
		return;
	}

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_Achievement>());
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineAchievementsPtr AchievementsInterface = Subsystem->GetAchievementsInterface();
	
	AchievementsInterface->QueryAchievements(
		*Identity->GetUniquePlayerId(0), FOnQueryAchievementsCompleteDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleQueryAchievements, WorldContextObject,OnDone));
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ULS_EOSSubsystem::HandleQueryAchievements(const FUniqueNetId& UserId, const bool bWasSuccessful,
	const UObject* WorldContextObject, FLSEOS_QueryAchievementsComplete OnDone)
{
	if (!bWasSuccessful)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_Achievement>());
		return;
	}

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_Achievement>());
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineAchievementsPtr AchievementsInterface = Subsystem->GetAchievementsInterface();
	
	TArray<FOnlineAchievement> AchievementsData;
	verify(AchievementsInterface->GetCachedAchievements(*Identity->GetUniquePlayerId(0), AchievementsData) == EOnlineCachedResult::Success);

	TArray<FLSEOS_Achievement> Results;
	for (const FOnlineAchievement& Data : AchievementsData)
	{
		FLSEOS_Achievement Achievement;
		Achievement.Id = Data.Id;
		Achievement.Progress = Data.Progress;
		Achievement.bUnlocked = Data.Progress >= 100.0f;

		FOnlineAchievementDesc AchievementDesc;
		verify(AchievementsInterface->GetCachedAchievementDescription(Data.Id, AchievementDesc) == EOnlineCachedResult::Success);
		
		Achievement.DisplayName = AchievementDesc.Title;
		Achievement.Description = Achievement.bUnlocked ? AchievementDesc.UnlockedDesc : AchievementDesc.LockedDesc;

		Results.Add(Achievement);
	}

	// ReSharper disable once CppExpressionWithoutSideEffects
	OnDone.ExecuteIfBound(true, Results);
}


