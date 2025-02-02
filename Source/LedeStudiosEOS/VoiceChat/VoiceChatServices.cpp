// Copyright June Rhodes. MIT Licensed.

#include "VoiceChatServices.h"
#include "LS_EOSSubsystem.h"
#include "VoiceChat.h"
#include "OnlineSubsystemUtils.h"

AVoiceChatServices::AVoiceChatServices()
{
}

void AVoiceChatServices::BeginPlay()
{
    Super::BeginPlay();

    IVoiceChat *VoiceChat = IVoiceChat::Get();
    if (VoiceChat == nullptr)
    {
        return;
    }

    this->VoiceChatUser = VoiceChat->CreateUser();

    SetupVoiceDelegates();
}

void AVoiceChatServices::BeginDestroy()
{
    IVoiceChat *VoiceChat = IVoiceChat::Get();
    if (VoiceChat && VoiceChatUser)
    {
        VoiceChat->ReleaseUser(VoiceChatUser);
        VoiceChatUser = nullptr;
    }

    Super::BeginDestroy();
}

void AVoiceChatServices::SetupVoiceDelegates()
{
    if (VoiceChatUser == nullptr)
    {
        return;
    }

    VoiceChatUser->OnVoiceChatAvailableAudioDevicesChanged().AddUObject(
        this,
        &AVoiceChatServices::HandleAudioDevicesChanged);
    VoiceChatUser->OnVoiceChatChannelJoined().AddUObject(this, &AVoiceChatServices::HandleVoiceChatChannelJoined);
    VoiceChatUser->OnVoiceChatChannelExited().AddUObject(this, &AVoiceChatServices::HandleVoiceChatChannelExited);
    VoiceChatUser->OnVoiceChatPlayerAdded().AddUObject(this, &AVoiceChatServices::HandleVoiceChatPlayerAdded);
    VoiceChatUser->OnVoiceChatPlayerRemoved().AddUObject(this, &AVoiceChatServices::HandleVoiceChatPlayerRemoved);
    VoiceChatUser->OnVoiceChatPlayerMuteUpdated().AddUObject(
        this,
        &AVoiceChatServices::HandleVoiceChatPlayerMuteUpdated);
    VoiceChatUser->OnVoiceChatPlayerTalkingUpdated().AddUObject(
        this,
        &AVoiceChatServices::HandleVoiceChatPlayerTalkingUpdated);
}

IVoiceChatUser *AVoiceChatServices::GetVoiceChatUser()
{
    return VoiceChatUser;
}

TArray<FString> AVoiceChatServices::GetAvailableInputDevices_Implementation()
{
    TArray<FVoiceChatDeviceInfo> VoiceChatDevices = VoiceChatUser->GetAvailableInputDeviceInfos();
    TArray<FString> InputDevices;
    InputDevices.Reserve(VoiceChatDevices.Num());
    for (auto &&Device : VoiceChatDevices)
    {
        InputDevices.Add(Device.DisplayName + TEXT("--") + Device.Id);
    }

    return InputDevices;
}

TArray<FString> AVoiceChatServices::GetAvailableOutputDevices_Implementation()
{
    TArray<FVoiceChatDeviceInfo> VoiceChatDevices = VoiceChatUser->GetAvailableOutputDeviceInfos();
    TArray<FString> OutputDevices;
    OutputDevices.Reserve(VoiceChatDevices.Num());
    for (auto &&Device : VoiceChatDevices)
    {
        OutputDevices.Add(Device.DisplayName + TEXT("--") + Device.Id);
    }

    return OutputDevices;
}

FString AVoiceChatServices::GetCurrentInputDevice_Implementation()
{
    const FVoiceChatDeviceInfo VoiceChatDevice = VoiceChatUser->GetInputDeviceInfo();
    FString DeviceInfo = VoiceChatDevice.DisplayName + TEXT("--") + VoiceChatDevice.Id;
    return DeviceInfo;
}

FString AVoiceChatServices::GetCurrentOutputDevice_Implementation()
{
    const FVoiceChatDeviceInfo VoiceChatDevice = VoiceChatUser->GetOutputDeviceInfo();
    FString DeviceInfo = VoiceChatDevice.DisplayName + TEXT("--") + VoiceChatDevice.Id;
    return DeviceInfo;
}

void AVoiceChatServices::SetInputDevice_Implementation(const FString &Device)
{
    FString DeviceName;
    FString DeviceId;
    Device.Split(TEXT("--"), &DeviceName, &DeviceId);
    VoiceChatUser->SetInputDeviceId(DeviceId);
}

void AVoiceChatServices::SetOutputDevice_Implementation(const FString &Device)
{
    FString DeviceName;
    FString DeviceId;
    Device.Split(TEXT("--"), &DeviceName, &DeviceId);
    VoiceChatUser->SetOutputDeviceId(DeviceId);
}

bool AVoiceChatServices::IsLoggedIn_Implementation()
{
    return VoiceChatUser->IsLoggedIn();
}

FString AVoiceChatServices::GetLoggedInPlayerName_Implementation()
{
    return VoiceChatUser->GetLoggedInPlayerName();
}

bool AVoiceChatServices::IsConnecting_Implementation()
{
    ULS_EOSSubsystem *CPPSubsystem =
        GetGameInstance() ? GetGameInstance()->GetSubsystem<ULS_EOSSubsystem>() : nullptr;
    return CPPSubsystem ? CPPSubsystem->IsConnecting() : false;
}

bool AVoiceChatServices::IsConnected_Implementation()
{
    ULS_EOSSubsystem *CPPSubsystem =
        GetGameInstance() ? GetGameInstance()->GetSubsystem<ULS_EOSSubsystem>() : nullptr;
    return CPPSubsystem ? CPPSubsystem->IsConnected() : false;
}

TArray<FString> AVoiceChatServices::GetJoinedChannels()
{
    const TArray<FString> JoinedChannels = VoiceChatUser->GetChannels();
    BP_GetJoinedChannels(JoinedChannels);
    return JoinedChannels;
}

TArray<FString> AVoiceChatServices::GetPlayersInChannel(const FString &ChannelName)
{
    const TArray<FString> Players = VoiceChatUser->GetPlayersInChannel(ChannelName);
    BP_GetPlayersInChannel(Players);
    return Players;
}

float AVoiceChatServices::GetInputVolume_Implementation()
{
    return VoiceChatUser->GetAudioInputVolume();
}

void AVoiceChatServices::SetInputVolume_Implementation(const float &Volume)
{
    VoiceChatUser->SetAudioInputVolume(Volume);
}

float AVoiceChatServices::GetOutputVolume_Implementation()
{
    return VoiceChatUser->GetAudioOutputVolume();
}

void AVoiceChatServices::SetOutputVolume_Implementation(const float &Volume)
{
    VoiceChatUser->SetAudioOutputVolume(Volume);
}

bool AVoiceChatServices::GetInputMuted_Implementation()
{
    return VoiceChatUser->GetAudioInputDeviceMuted();
}

void AVoiceChatServices::SetInputMuted_Implementation(bool bMuted)
{
    VoiceChatUser->SetAudioInputDeviceMuted(bMuted);
}

bool AVoiceChatServices::GetOutputMuted_Implementation()
{
    return VoiceChatUser->GetAudioOutputDeviceMuted();
}

void AVoiceChatServices::SetOutputMuted_Implementation(bool bMuted)
{
    VoiceChatUser->SetAudioOutputDeviceMuted(bMuted);
}

ELSEOS_VoiceChatTransmitMode AVoiceChatServices::GetTransmitMode_Implementation()
{
    const EVoiceChatTransmitMode TransmitMode = VoiceChatUser->GetTransmitMode();
    ELSEOS_VoiceChatTransmitMode OutTransmitMode;
    switch (TransmitMode)
    {
    case EVoiceChatTransmitMode::All:
        OutTransmitMode = ELSEOS_VoiceChatTransmitMode::All;
        break;
    case EVoiceChatTransmitMode::Channel:
        OutTransmitMode = ELSEOS_VoiceChatTransmitMode::Channel;
        break;
    case EVoiceChatTransmitMode::None:
    default:
        OutTransmitMode = ELSEOS_VoiceChatTransmitMode::None;
    }

    return OutTransmitMode;
}

void AVoiceChatServices::SetTransmitMode_Implementation(const ELSEOS_VoiceChatTransmitMode &TransmitMode)
{
    switch (TransmitMode)
    {
    case ELSEOS_VoiceChatTransmitMode::All: {
        VoiceChatUser->TransmitToAllChannels();
        break;
    }
    case ELSEOS_VoiceChatTransmitMode::Channel: {
        // Default to transmitting to the first channel the user is in
        TArray<FString> ChannelNames = GetJoinedChannels();
        if (ChannelNames.IsValidIndex(0))
        {
            /**
             * @note EOS Can transmit to several specific channels at once. This example is just using the first
             * channel the user is in. If you want a user to transmit to multiple voice channels at once, you'll
             * need to pick the specific channel names you want the user to transmit to and then call
             * TransmitToSpecificChannel for each channel.
             *
             * Additionally, you need to call TransmitToNoChannels first to clear the slate before you specify a channel
             * to transmit to.
             */
            VoiceChatUser->TransmitToNoChannels();
            VoiceChatUser->TransmitToSpecificChannel(ChannelNames[0]);
        }
        break;
    }
    case ELSEOS_VoiceChatTransmitMode::None:
    default: {
        VoiceChatUser->TransmitToNoChannels();
    }
    }
}

bool AVoiceChatServices::IsPlayerMuted(const FString &PlayerName)
{
    return VoiceChatUser->IsPlayerMuted(PlayerName);
}

bool AVoiceChatServices::IsPlayerTalking(const FString &PlayerName)
{
    return VoiceChatUser->IsPlayerTalking(PlayerName);
}

void AVoiceChatServices::HandleAudioDevicesChanged()
{
    OnAudioDevicesChanged.Broadcast();
}

void AVoiceChatServices::HandleVoiceChatChannelJoined(const FString &ChannelName)
{
    OnVoiceChatChannelJoined.Broadcast(ChannelName);
}

void AVoiceChatServices::HandleVoiceChatChannelExited(const FString &ChannelName, const FVoiceChatResult &Result)
{
    const FLSEOS_VoiceChatResult VoiceChatResultCPP = FVoiceChatResult(Result);
    OnVoiceChatChannelExited.Broadcast(ChannelName, VoiceChatResultCPP);
}

void AVoiceChatServices::HandleVoiceChatPlayerAdded(const FString &ChannelName, const FString &PlayerName)
{
    OnVoiceChatPlayerAdded.Broadcast(ChannelName, PlayerName);
}

void AVoiceChatServices::HandleVoiceChatPlayerRemoved(const FString &ChannelName, const FString &PlayerName)
{
    OnVoiceChatPlayerRemoved.Broadcast(ChannelName, PlayerName);
}

void AVoiceChatServices::HandleVoiceChatPlayerMuteUpdated(
    const FString &ChannelName,
    const FString &PlayerName,
    bool bIsMuted)
{
    OnVoiceChatPlayerMuteUpdated.Broadcast(ChannelName, PlayerName, bIsMuted);
}

void AVoiceChatServices::HandleVoiceChatPlayerTalkingUpdated(
    const FString &ChannelName,
    const FString &PlayerName,
    bool bIsTalking)
{
    OnVoiceChatPlayerTalkingUpdated.Broadcast(ChannelName, PlayerName, bIsTalking);
}