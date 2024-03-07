// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

bool ULS_EOSSubsystem::IsLoggedIn(const UObject* WorldContextObject)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return false;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	return Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn;
}

FString ULS_EOSSubsystem::GetLoggedInUsername(const UObject* WorldContextObject)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return TEXT("");
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		const TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
		if (Id.IsValid())
		{
			const TSharedPtr<FUserOnlineAccount> Account = Identity->GetUserAccount(*Id);
			if (Account.IsValid())
			{
				if (Account->GetDisplayName().IsEmpty())
				{
					return TEXT("(No Username Set, but you are Logged In)");
				}
				else
				{
					return Account->GetDisplayName();
				}
			}
		}
	}

	return TEXT("Not Logged In");
}

FString ULS_EOSSubsystem::GetLoggedInProductUserId(const UObject* WorldContextObject)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return TEXT("");
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		const TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
		if (Id.IsValid())
		{
			const TSharedPtr<FUserOnlineAccount> Account = Identity->GetUserAccount(*Id);
			if (Account.IsValid())
			{
				FString OutAttr;
				if (Account->GetUserAttribute(TEXT("productUserId"), OutAttr))
				{
					return OutAttr;
				}
			}
		}
	}

	return TEXT("");
}

FString ULS_EOSSubsystem::GetLoggedInEpicAccountId(const UObject* WorldContextObject)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return TEXT("");
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		const TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
		if (Id.IsValid())
		{
			const TSharedPtr<FUserOnlineAccount> Account = Identity->GetUserAccount(*Id);
			if (Account.IsValid())
			{
				FString OutTypes;
				if (Account->GetUserAttribute(TEXT("externalAccountTypes"), OutTypes))
				{
					TArray<FString> TypesArr;
					OutTypes.ParseIntoArray(TypesArr, TEXT(","));
					if (TypesArr.Contains("epic"))
					{
						FString OutAttr;
						if (Account->GetUserAttribute(TEXT("externalAccount.epic.id"), OutAttr))
						{
							return OutAttr;
						}
					}
				}
			}
		}
	}

	return TEXT("");
}

FString ULS_EOSSubsystem::GetLoggedInAuthAttribute(const UObject* WorldContextObject, const FString& InAuthAttrName)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return TEXT("");
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		const TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
		if (Id.IsValid())
		{
			const TSharedPtr<FUserOnlineAccount> Account = Identity->GetUserAccount(*Id);
			if (Account.IsValid())
			{
				FString OutAttr;
				if (Account->GetAuthAttribute(InAuthAttrName, OutAttr))
				{
					return OutAttr;
				}
			}
		}
	}
	
	return TEXT("");
}

FUniqueNetIdRepl ULS_EOSSubsystem::GetLoggedInUserId(const UObject* WorldContextObject)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return FUniqueNetIdRepl();
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		const TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
		if (Id.IsValid())
		{
			return FUniqueNetIdRepl(Id);
		}
	}

	return FUniqueNetIdRepl();
}

bool ULS_EOSSubsystem::CanLinkToEpicGamesAccount(const UObject* WorldContextObject)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return false;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		const TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
		if (Id.IsValid())
		{
			const TSharedPtr<FUserOnlineAccount> Account = Identity->GetUserAccount(*Id);
			if (Account.IsValid())
			{
				FString OutVal;
				if (Account->GetAuthAttribute(TEXT("crossPlatform.canLink"), OutVal))
				{
					return OutVal == TEXT("true");
				}
			}
		}
	}
	
	return false;
}

void ULS_EOSSubsystem::StartLogin(const UObject* WorldContextObject, const int32 LocalUserNum, FLSEOS_LoginComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TEXT("Call didn't start"));
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	// note: This example code only supports running one Login process at once; if you want to support
	// multiple in flight you probably need to make a latent blueprint node.
	if (this->LoginDelegateHandle.IsValid())
	{
		OnDone.ExecuteIfBound(false, TEXT("Call didn't start"));
		return;
	}
	
	this->LoginDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(
		LocalUserNum, FOnLoginComplete::FDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleLoginComplete,WorldContextObject,OnDone));
	
	if (!Identity->AutoLogin(LocalUserNum))
	{
		OnDone.ExecuteIfBound(false, TEXT("Call didn't start"));
	}
}

void ULS_EOSSubsystem::HandleLoginComplete(const int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error, const UObject* WorldContextObject, FLSEOS_LoginComplete OnDone)
{
	OnDone.ExecuteIfBound(bWasSuccessful, Error);

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity == nullptr)
	{
		return;
	}

	Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, this->LoginDelegateHandle);
	this->LoginDelegateHandle.Reset();
}

void ULS_EOSSubsystem::StartLogout(const UObject* WorldContextObject, FLSEOS_LogoutComplete OnDone)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	// note: This example code only supports running one Logout process at once; if you want to support
	// multiple in flight you probably need to make a latent blueprint node.
	if (this->LogoutDelegateHandle.IsValid())
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	this->LogoutDelegateHandle = Identity->AddOnLogoutCompleteDelegate_Handle(
		0, FOnLogoutComplete::FDelegate::CreateUObject(
			this, &ULS_EOSSubsystem::HandleLogoutComplete,WorldContextObject,OnDone));

	if (!Identity->Logout(0))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ULS_EOSSubsystem::HandleLogoutComplete(int32 LocalUserNum, bool bWasSuccessful,
	const UObject* WorldContextObject, FLSEOS_LogoutComplete OnDone)
{
	OnDone.ExecuteIfBound(bWasSuccessful);

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity == nullptr)
	{
		return;
	}

	Identity->ClearOnLogoutCompleteDelegate_Handle(0, this->LogoutDelegateHandle);
	this->LogoutDelegateHandle.Reset();
}