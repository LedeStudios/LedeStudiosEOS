// Copyright Lede Studios. All Rights Reserved.


#include "LS_EOSSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void ULS_EOSSubsystem::PostInitProperties()
{
	Super::PostInitProperties();

    if (this->HasAnyFlags(RF_ClassDefaultObject))
    {
        return;
    }

    check(this->GetWorld() != nullptr);

    const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem != nullptr && Subsystem->GetSubsystemName() == FName(TEXT("RedpointEOS")))
    {
        IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
        const IOnlinePartyPtr Party = Subsystem->GetPartyInterface();
        const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
        const IOnlinePresencePtr Presence = Subsystem->GetPresenceInterface();
        const IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();
        
        if (Party.IsValid())
        {
            Party->AddOnPartyInvitesChangedDelegate_Handle(
                FOnPartyInvitesChangedDelegate::CreateUObject(this, &ULS_EOSSubsystem::OnPartyInvitesChanged));
            Party->AddOnPartyDataReceivedDelegate_Handle(
                FOnPartyDataReceivedDelegate::CreateUObject(this, &ULS_EOSSubsystem::OnPartyDataReceived));
        }
        
        if (Session.IsValid())
        {
            Session->AddOnSessionUserInviteAcceptedDelegate_Handle(FOnSessionUserInviteAcceptedDelegate::CreateUObject(
                this,
                &ULS_EOSSubsystem::OnSessionUserInviteAccepted));
            Session->AddOnJoinSessionCompleteDelegate_Handle(
                FOnJoinSessionCompleteDelegate::CreateUObject(this, &ULS_EOSSubsystem::OnSessionJoinedViaOverlay));
        }
        
        if (Presence.IsValid())
        {
            Presence->AddOnPresenceReceivedDelegate_Handle(
                FOnPresenceReceivedDelegate::CreateUObject(this, &ULS_EOSSubsystem::OnPresenceReceived));
        }
        
        if (Friends.IsValid())
        {
            Friends->AddOnFriendsChangeDelegate_Handle(0, 
                FOnFriendsChangeDelegate::CreateUObject(this, &ULS_EOSSubsystem::OnFriendsChange));
        }
    }
}

FString ULS_EOSSubsystem::UniqueNetIdToString(const FUniqueNetIdRepl& UniqueNetId)
{
    return UniqueNetId.ToString();
}

