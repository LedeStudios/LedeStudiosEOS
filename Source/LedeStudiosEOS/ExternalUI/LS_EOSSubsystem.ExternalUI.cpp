// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"


void ULS_EOSSubsystem::ShowFriendsUI(const UObject* WorldContextObject) const
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	check(Subsystem != nullptr);
	const IOnlineExternalUIPtr ExternalUI = Subsystem->GetExternalUIInterface();

	if (ExternalUI.IsValid())
	{
		ExternalUI->ShowFriendsUI(0);
	}
}

void ULS_EOSSubsystem::ShowInviteUI(const UObject* WorldContextObject, const FName SessionName) const
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	check(Subsystem != nullptr);
	const IOnlineExternalUIPtr ExternalUI = Subsystem->GetExternalUIInterface();

	if (ExternalUI.IsValid())
	{
		ExternalUI->ShowInviteUI(0, SessionName);
	}
}