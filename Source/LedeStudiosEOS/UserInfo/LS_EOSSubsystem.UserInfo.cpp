// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void ULS_EOSSubsystem::QueryUserInfo(const UObject* WorldContextObject, FString ProductUserIdInput,
	FLSEOS_QueryUserInfoComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(TEXT("Subsystem not available."));
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineUserPtr User = Subsystem->GetUserInterface();

	const TSharedPtr<const FUniqueNetId> UniqueNetId = Identity->CreateUniquePlayerId(ProductUserIdInput);
	if (UniqueNetId == nullptr)
	{
		OnDone.ExecuteIfBound(TEXT("The ID you entered was invalid."));
		return;
	}

	// note: This example code only supports running one QueryUserInfo process at once; if you want to support
	// multiple in flight you probably need to make a latent blueprint node.
	if (this->QueryUserInfoDelegateHandle.IsValid())
	{
		OnDone.ExecuteIfBound(TEXT("Already a QueryUserInfo running."));
		return;
	}

	this->QueryUserInfoDelegateHandle = User->AddOnQueryUserInfoCompleteDelegate_Handle(
		0,
		FOnQueryUserInfoComplete::FDelegate::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleQueryUserInfoComplete,
			WorldContextObject,
			OnDone));

	TArray<TSharedRef<const FUniqueNetId>> UserIds;
	UserIds.Add(UniqueNetId.ToSharedRef());
	User->QueryUserInfo(0, UserIds);
}

void ULS_EOSSubsystem::HandleQueryUserInfoComplete(int32 LocalUserNum, bool bWasSuccessful,
	const TArray<TSharedRef<const FUniqueNetId>>& UserIds, const FString& ErrorStr, const UObject* WorldContextObject,
	FLSEOS_QueryUserInfoComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(TEXT("Subsystem not available."));
		return;
	}
	const IOnlineUserPtr User = Subsystem->GetUserInterface();
	if (User == nullptr)
	{
		OnDone.ExecuteIfBound(TEXT("Interface not available."));
		return;
	}

	if (!bWasSuccessful)
	{
		OnDone.ExecuteIfBound(FString::Printf(TEXT("Error: %s"), *ErrorStr));
	}
	else
	{
		if (UserIds.Num() == 0)
		{
			OnDone.ExecuteIfBound(TEXT("No such user was found."));
		}
		else if (UserIds.Num() == 1)
		{
			const TSharedRef<const FUniqueNetId> Id = UserIds[0];
			const TSharedPtr<FOnlineUser> UserAcc = User->GetUserInfo(0, *Id);

			if (UserAcc.IsValid())
			{
				OnDone.ExecuteIfBound(FString::Printf(TEXT("%s -> %s"), *Id->ToString(), *UserAcc->GetDisplayName()));
			}
			else
			{
				OnDone.ExecuteIfBound(FString::Printf(TEXT("%s -> (bug!!)"), *Id->ToString()));
			}
		}
		else
		{
			OnDone.ExecuteIfBound(TEXT("Bug: Too many user IDs returned!"));
		}
	}

	User->ClearOnQueryUserInfoCompleteDelegate_Handle(0, this->QueryUserInfoDelegateHandle);
	this->QueryUserInfoDelegateHandle.Reset();
}

void ULS_EOSSubsystem::QueryUserIdMapping(const UObject* WorldContextObject, FString DisplayNameInput,
	FLSEOS_QueryUserIdMappingComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(TEXT("Subsystem not available."));
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineUserPtr User = Subsystem->GetUserInterface();

	User->QueryUserIdMapping(
		*Identity->GetUniquePlayerId(0),
		DisplayNameInput,
		IOnlineUser::FOnQueryUserMappingComplete::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleQueryUserMappingComplete,
			WorldContextObject,
			OnDone));
}

void ULS_EOSSubsystem::HandleQueryUserMappingComplete(const bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& DisplayNameOrEmail, const FUniqueNetId& FoundUserId, const FString& Error,
	const UObject* WorldContextObject, FLSEOS_QueryUserIdMappingComplete OnDone)
{
	if (!bWasSuccessful)
	{
		OnDone.ExecuteIfBound(FString::Printf(TEXT("Error: %s"), *Error));
		return;
	}

	OnDone.ExecuteIfBound(FString::Printf(TEXT("Found user: %s"), *FoundUserId.ToString()));
}

void ULS_EOSSubsystem::QueryExternalIdMappings(const UObject* WorldContextObject, FString PlatformType,
const FString ExternalIdsStr, FLSEOS_QueryExternalIdMappingsComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(TEXT("Subsystem not available."));
		return;
	}
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	const IOnlineUserPtr User = Subsystem->GetUserInterface();

	TArray<FString> ExternalIdsTemp, ExternalIds;
	ExternalIdsStr.ParseIntoArray(ExternalIdsTemp, TEXT("\n"));
	for (auto ExternalIdTemp : ExternalIdsTemp)
	{
		ExternalIds.Add(ExternalIdTemp.TrimStartAndEnd());
	}

	FExternalIdQueryOptions Opts = {};
	Opts.AuthType = PlatformType;
	Opts.bLookupByDisplayName = false;

	User->QueryExternalIdMappings(
		*Identity->GetUniquePlayerId(0),
		Opts,
		ExternalIds,
		IOnlineUser::FOnQueryExternalIdMappingsComplete::CreateUObject(
			this,
			&ULS_EOSSubsystem::HandleQueryExternalIdMappings,
			WorldContextObject,
			OnDone));
}

void ULS_EOSSubsystem::HandleQueryExternalIdMappings(bool bWasSuccessful, const FUniqueNetId& UserId,
	const FExternalIdQueryOptions& QueryOptions, const TArray<FString>& ExternalIds, const FString& Error,
	const UObject* WorldContextObject, FLSEOS_QueryExternalIdMappingsComplete OnDone)
{
	if (!bWasSuccessful)
	{
		OnDone.ExecuteIfBound(FString::Printf(TEXT("Error: %s"), *Error));
		return;
	}

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(TEXT("Subsystem not available."));
		return;
	}
	const IOnlineUserPtr User = Subsystem->GetUserInterface();
	if (User == nullptr)
	{
		OnDone.ExecuteIfBound(TEXT("Interface not available."));
		return;
	}

	TArray<FString> Lines;
	for (auto ExternalId : ExternalIds)
	{
		if (FUniqueNetIdPtr EUID = User->GetExternalIdMapping(QueryOptions, ExternalId); EUID.IsValid())
		{
			Lines.Add(FString::Printf(TEXT("%s = %s"), *ExternalId, *EUID->ToString()));
		}
		else
		{
			Lines.Add(FString::Printf(TEXT("%s = not found"), *ExternalId));
		}
	}
	OnDone.ExecuteIfBound(FString::Join(Lines, TEXT("\n")));
}
