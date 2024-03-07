// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"


void FLSEOS_VoiceChatChannel3dProperties::ConvertAttenuationModelToBlueprintableResult(
	const EVoiceChatAttenuationModel& AttenuationModel, ELSEOS_VoiceChatAttenuationModel& OutAttenuationModel)
{
	switch (AttenuationModel)
	{
	case EVoiceChatAttenuationModel::InverseByDistance:
		OutAttenuationModel = ELSEOS_VoiceChatAttenuationModel::InverseByDistance;
		break;
	case EVoiceChatAttenuationModel::LinearByDistance:
		OutAttenuationModel = ELSEOS_VoiceChatAttenuationModel::LinearByDistance;
		break;
	case EVoiceChatAttenuationModel::ExponentialByDistance:
		OutAttenuationModel = ELSEOS_VoiceChatAttenuationModel::ExponentialByDistance;
		break;
	case EVoiceChatAttenuationModel::None:
	default:
		OutAttenuationModel = ELSEOS_VoiceChatAttenuationModel::None;
	}
}

inline void FLSEOS_VoiceChatChannel3dProperties::ConvertBlueprintableAttenuationModelToAttenuationModel(
	const ELSEOS_VoiceChatAttenuationModel& AttenuationModel, EVoiceChatAttenuationModel& OutAttenuationModel)
{
	switch (AttenuationModel)
	{
	case ELSEOS_VoiceChatAttenuationModel::InverseByDistance:
		OutAttenuationModel = EVoiceChatAttenuationModel::InverseByDistance;
		break;
	case ELSEOS_VoiceChatAttenuationModel::LinearByDistance:
		OutAttenuationModel = EVoiceChatAttenuationModel::LinearByDistance;
		break;
	case ELSEOS_VoiceChatAttenuationModel::ExponentialByDistance:
		OutAttenuationModel = EVoiceChatAttenuationModel::ExponentialByDistance;
		break;
	case ELSEOS_VoiceChatAttenuationModel::None:
	default:
		OutAttenuationModel = EVoiceChatAttenuationModel::None;
	}
}

void FLSEOS_VoiceChatChannel3dProperties::ConvertBlueprintableChannelTypeToChannelType(
	const ELSEOS_VoiceChatChannelType& ChannelType, EVoiceChatChannelType& OutChannelType)
{
	switch (ChannelType)
	{
	case ELSEOS_VoiceChatChannelType::NonPositional:
		OutChannelType = EVoiceChatChannelType::NonPositional;
		break;
	case ELSEOS_VoiceChatChannelType::Positional:
		OutChannelType = EVoiceChatChannelType::Positional;
		break;
	case ELSEOS_VoiceChatChannelType::Echo:;
	default:
		OutChannelType = EVoiceChatChannelType::Echo;
	}
}

FVoiceChatChannel3dProperties FLSEOS_VoiceChatChannel3dProperties::
	ConvertBlueprintableChatChannelPropertiesToChatChannelProperties(
	const FLSEOS_VoiceChatChannel3dProperties& ChatChannel3dProperties)
{
	FVoiceChatChannel3dProperties VoiceChatChannel3dProperties;
	VoiceChatChannel3dProperties.Rolloff = ChatChannel3dProperties.Rolloff;
	VoiceChatChannel3dProperties.MinDistance = ChatChannel3dProperties.MinDistance;
	VoiceChatChannel3dProperties.MaxDistance = ChatChannel3dProperties.MaxDistance;
	ConvertBlueprintableAttenuationModelToAttenuationModel(
		ChatChannel3dProperties.AttenuationModel,
		VoiceChatChannel3dProperties.AttenuationModel);

	return VoiceChatChannel3dProperties;
}

void FLSEOS_VoiceChatResult::ConvertVoiceChatResultToBlueprintableResult(
	const EVoiceChatResult& VoiceChatResult,
	ELSEOS_VoiceChatResult& OutResult)
{
	switch (VoiceChatResult)
	{
	case EVoiceChatResult::Success:
		OutResult = ELSEOS_VoiceChatResult::Success;
		break;
	case EVoiceChatResult::InvalidState:
		OutResult = ELSEOS_VoiceChatResult::InvalidState;
		break;
	case EVoiceChatResult::NotInitialized:
		OutResult = ELSEOS_VoiceChatResult::NotInitialized;
		break;
	case EVoiceChatResult::NotConnected:
		OutResult = ELSEOS_VoiceChatResult::NotConnected;
		break;
	case EVoiceChatResult::NotLoggedIn:
		OutResult = ELSEOS_VoiceChatResult::NotLoggedIn;
		break;
	case EVoiceChatResult::NotPermitted:
		OutResult = ELSEOS_VoiceChatResult::NotPermitted;
		break;
	case EVoiceChatResult::Throttled:
		OutResult = ELSEOS_VoiceChatResult::Throttled;
		break;
	case EVoiceChatResult::InvalidArgument:
		OutResult = ELSEOS_VoiceChatResult::InvalidArgument;
		break;
	case EVoiceChatResult::CredentialsInvalid:
		OutResult = ELSEOS_VoiceChatResult::CredentialsInvalid;
		break;
	case EVoiceChatResult::CredentialsExpired:
		OutResult = ELSEOS_VoiceChatResult::CredentialsExpired;
		break;
	case EVoiceChatResult::ClientTimeout:
		OutResult = ELSEOS_VoiceChatResult::ClientTimeout;
		break;
	case EVoiceChatResult::ServerTimeout:
		OutResult = ELSEOS_VoiceChatResult::ServerTimeout;
		break;
	case EVoiceChatResult::DnsFailure:
		OutResult = ELSEOS_VoiceChatResult::DnsFailure;
		break;
	case EVoiceChatResult::ConnectionFailure:
		OutResult = ELSEOS_VoiceChatResult::ConnectionFailure;
		break;
	case EVoiceChatResult::ImplementationError:
	default:
		OutResult = ELSEOS_VoiceChatResult::ImplementationError;
	}
}

void ULS_EOSSubsystem::LoginToVoice(AVoiceChatServices* VoiceChatService, FLSEOS_OnVoiceChatLoginComplete OnDone)
{
	FLSEOS_VoiceChatResult VoiceChatResult;

    if (!IsValid(VoiceChatService))
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatService Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

    if (VoiceChatService->GetVoiceChatUser() == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatUser was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Subsystem Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Identity Interface was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::CredentialsInvalid;
        VoiceChatResult.ErrorDescription = TEXT("UniqueNetId of player 0 was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

	const IVoiceChat *VoiceChat = IVoiceChat::Get();
    if (VoiceChat == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChat Interface was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    const FPlatformUserId PlatformUserId = Identity->GetPlatformUserIdFromUniqueNetId(*Identity->GetUniquePlayerId(0));

    VoiceChatService->GetVoiceChatUser()->Login(
        PlatformUserId,
        Identity->GetUniquePlayerId(0)->ToString(),
        TEXT(""),
        FOnVoiceChatLoginCompleteDelegate::CreateUObject(
            this,
            &ULS_EOSSubsystem::HandleVoiceChatLoginComplete,
            OnDone));
}

void ULS_EOSSubsystem::HandleVoiceChatLoginComplete(const FString& PlayerName, const FVoiceChatResult& Result,
	FLSEOS_OnVoiceChatLoginComplete OnDone)
{
	const FLSEOS_VoiceChatResult VoiceChatResult = FLSEOS_VoiceChatResult(Result);
	OnDone.ExecuteIfBound(PlayerName, VoiceChatResult);
}

void ULS_EOSSubsystem::LogoutOfVoice(AVoiceChatServices* VoiceChatService, FLSEOS_OnVoiceChatLogoutComplete OnDone)
{
	FLSEOS_VoiceChatResult VoiceChatResult;

    if (VoiceChatService == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatService Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

    if (VoiceChatService->GetVoiceChatUser() == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatUser was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Subsystem Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Identity Interface was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::CredentialsInvalid;
        VoiceChatResult.ErrorDescription = TEXT("UniqueNetId of player 0 was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    VoiceChatService->GetVoiceChatUser()->Logout(FOnVoiceChatLogoutCompleteDelegate::CreateUObject(
        this,
        &ULS_EOSSubsystem::HandleVoiceChatLogoutComplete,
        VoiceChatService,
        OnDone));
}

void ULS_EOSSubsystem::HandleVoiceChatLogoutComplete(const FString& PlayerName, const FVoiceChatResult& Result,
	AVoiceChatServices* VoiceChatService, FLSEOS_OnVoiceChatLogoutComplete OnDone)
{
	VoiceChatService->Destroy();
	const FLSEOS_VoiceChatResult VoiceChatResult = FLSEOS_VoiceChatResult(Result);
	OnDone.ExecuteIfBound(PlayerName, VoiceChatResult);
}

void ULS_EOSSubsystem::JoinVoiceChannel(AVoiceChatServices* VoiceChatService, const FString& ChannelName,
	const FString& ChannelCredentials, const ELSEOS_VoiceChatChannelType ChannelType,
	const FLSEOS_VoiceChatChannel3dProperties Channel3dProperties, FLSEOS_OnVoiceChatChannelJoinedComplete OnDone)
{
	FLSEOS_VoiceChatResult VoiceChatResult;

    if (VoiceChatService == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatService Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

    if (VoiceChatService->GetVoiceChatUser() == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatUser was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Subsystem Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Identity Interface was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::CredentialsInvalid;
        VoiceChatResult.ErrorDescription = TEXT("UniqueNetId of player 0 was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    IVoiceChat *VoiceChat = IVoiceChat::Get();
    if (VoiceChat == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = ELSEOS_VoiceChatResult::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChat Interface was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    EVoiceChatChannelType VoiceChatChannelType;
    FLSEOS_VoiceChatChannel3dProperties::ConvertBlueprintableChannelTypeToChannelType(ChannelType, VoiceChatChannelType);
    const TOptional<FVoiceChatChannel3dProperties> ChatChannel3dProperties =
        FLSEOS_VoiceChatChannel3dProperties::ConvertBlueprintableChatChannelPropertiesToChatChannelProperties(
            Channel3dProperties);

    VoiceChat->JoinChannel(
        ChannelName,
        ChannelCredentials,
        VoiceChatChannelType,
        FOnVoiceChatChannelJoinCompleteDelegate::CreateUObject(
            this,
            &ULS_EOSSubsystem::HandleVoiceChatJoinChannelComplete,
            OnDone),
        ChatChannel3dProperties);
}

void ULS_EOSSubsystem::HandleVoiceChatJoinChannelComplete(const FString& ChannelName, const FVoiceChatResult& Result,
	FLSEOS_OnVoiceChatChannelJoinedComplete OnDone)
{
	const FLSEOS_VoiceChatResult VoiceChatResult = FLSEOS_VoiceChatResult(Result);
	OnDone.ExecuteIfBound(ChannelName, VoiceChatResult);
}

bool ULS_EOSSubsystem::IsConnected() const
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
	if (Subsystem == nullptr)
	{
		return false;
	}

	const IVoiceChat *VoiceChat = IVoiceChat::Get();
	return VoiceChat ? VoiceChat->IsConnected() : false;
}

bool ULS_EOSSubsystem::IsConnecting() const
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
	if (Subsystem == nullptr)
	{
		return false;
	}

	const IVoiceChat *VoiceChat = IVoiceChat::Get();
	return VoiceChat ? VoiceChat->IsConnecting() : false;
}