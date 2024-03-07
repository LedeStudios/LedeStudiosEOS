// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineAvatarInterface.h"

void ULS_EOSSubsystem::GetAvatar(const int32 LocalUserId, const FUniqueNetIdRepl& TargetUserId,
	const UTexture* DefaultTexture, FLSEOS_GetAvatarComplete OnDone)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, DefaultTexture);
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity == nullptr)
	{
		OnDone.ExecuteIfBound(false, DefaultTexture);
		return;
	}

	if (Identity->GetUniquePlayerId(LocalUserId).IsValid() == false)
	{
		OnDone.ExecuteIfBound(false, DefaultTexture);
		return;
	}

	const TSharedPtr<IOnlineAvatar> Avatar = Online::GetAvatarInterface(Subsystem);
	if (Avatar.IsValid() == false)
	{
		OnDone.ExecuteIfBound(false, DefaultTexture);
		return;
	}

	Avatar->GetAvatar(*Identity->GetUniquePlayerId(LocalUserId), *TargetUserId,DefaultTexture,
		FOnGetAvatarComplete::CreateUObject(this, &ULS_EOSSubsystem::HandleGetAvatarComplete, OnDone));
}

void ULS_EOSSubsystem::HandleGetAvatarComplete(const bool bWasSuccessful, TSoftObjectPtr<UTexture> ResultTexture, FLSEOS_GetAvatarComplete OnDone)
{
	OnDone.ExecuteIfBound(bWasSuccessful, ResultTexture);
}


