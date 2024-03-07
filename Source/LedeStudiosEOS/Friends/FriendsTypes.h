// Copyright Lede Studios. All Rights Reserved.

#pragma once

#include "Containers/Set.h"
#include "CoreMinimal.h"
#include "GameFramework/OnlineReplStructs.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "FriendsTypes.generated.h"

UCLASS(BlueprintType)
class LEDESTUDIOSEOS_API ULSEOS_Friend : public UObject
{
    GENERATED_BODY()

public:
    TSharedPtr<FOnlineFriend> Friend;

    UFUNCTION(BlueprintPure)
    FUniqueNetIdRepl GetUserId() const
    {
        return FUniqueNetIdRepl(*Friend->GetUserId());
    }

    UFUNCTION(BlueprintPure)
    FString GetUserIdString() const
    {
        return Friend->GetUserId()->ToString();
    }

    UFUNCTION(BlueprintPure)
    FName GetUserIdType() const
    {
        return Friend->GetUserId()->GetType();
    }

    UFUNCTION(BlueprintPure)
    FString GetDisplayName() const
    {
        return Friend->GetDisplayName();
    }

    UFUNCTION(BlueprintPure)
    FString GetRealName() const
    {
        return Friend->GetRealName();
    }

    UFUNCTION(BlueprintPure)
    FString GetWrappedFriendsSummary() const
    {
        TArray<FString> Summary;
        FString SubsystemNamesStr;
        if (Friend->GetUserAttribute(TEXT("eosSynthetic.subsystemNames"), SubsystemNamesStr))
        {
            TArray<FString> SubsystemNames;
            SubsystemNamesStr.ParseIntoArray(SubsystemNames, TEXT(","));
            for (const auto &SubsystemName : SubsystemNames)
            {
                FString Id, RealName, DisplayName;
                Friend->GetUserAttribute(FString::Printf(TEXT("eosSynthetic.friend.%s.id"), *SubsystemName), Id);
                Friend->GetUserAttribute(
                    FString::Printf(TEXT("eosSynthetic.friend.%s.realName"), *SubsystemName),
                    RealName);
                Friend->GetUserAttribute(
                    FString::Printf(TEXT("eosSynthetic.friend.%s.displayName"), *SubsystemName),
                    DisplayName);
                Summary.Add(FString::Printf(
                    TEXT("subsystem: %s id: %s real name: %s display name: %s"),
                    *SubsystemName,
                    *Id,
                    *RealName,
                    *DisplayName));
            }
        }
        return FString::Join(Summary, TEXT("\n"));
    }

    UFUNCTION(BlueprintCallable)
    FString ToString() const
    {
        if (Friend == nullptr)
        {
            return TEXT("");
        }
        return Friend->GetDisplayName();
    }

    UFUNCTION(BlueprintPure)
    bool CanAcceptInvite() const
    {
        if (Friend == nullptr)
        {
            return false;
        }
        return Friend->GetInviteStatus() == EInviteStatus::PendingInbound;
    }

    UFUNCTION(BlueprintPure)
    bool CanDeleteFriend() const
    {
        if (Friend == nullptr)
        {
            return false;
        }

        return Friend->GetInviteStatus() == EInviteStatus::PendingOutbound ||
               Friend->GetInviteStatus() == EInviteStatus::Accepted;
    }

    UFUNCTION(BlueprintPure)
    bool CanRejectInvite() const
    {
        if (Friend == nullptr)
        {
            return false;
        }

        return Friend->GetInviteStatus() == EInviteStatus::PendingInbound;
    }

    UFUNCTION(BlueprintPure)
    FString GetInviteStatus() const
    {
        if (Friend == nullptr)
        {
            return TEXT("Unknown");
        }

        switch (Friend->GetInviteStatus())
        {
        case EInviteStatus::Accepted:
            return TEXT("Accepted");
        case EInviteStatus::Blocked:
            return TEXT("Blocked");
        case EInviteStatus::PendingInbound:
            return TEXT("Pending Inbound");
        case EInviteStatus::PendingOutbound:
            return TEXT("Pending Outbound");
        case EInviteStatus::Suggested:
            return TEXT("Suggested");
        default:
            return TEXT("Unknown");
        }
    }

    UFUNCTION(BlueprintPure)
    FString GetPresenceLastOnline() const
    {
        if (Friend == nullptr)
        {
            return TEXT("");
        }

        return Friend->GetPresence().LastOnline.ToString();
    }

    UFUNCTION(BlueprintPure)
    FString GetPresenceSessionID() const
    {
        if (Friend == nullptr || !Friend->GetPresence().SessionId.IsValid())
        {
            return TEXT("");
        }

        return Friend->GetPresence().SessionId->ToString();
    }

    UFUNCTION(BlueprintPure)
    FString GetPresenceStatusProperties() const
    {
        if (Friend == nullptr)
        {
            return TEXT("");
        }

        TArray<FString> Results;
        for (const auto &Prop : Friend->GetPresence().Status.Properties)
        {
            Results.Add(FString::Printf(TEXT("%s=%s"), *Prop.Key, *Prop.Value.ToString()));
        }
        return FString::Join(Results, TEXT("\n"));
    }

    UFUNCTION(BlueprintPure)
    FString GetPresenceStatusState() const
    {
        if (Friend == nullptr)
        {
            return TEXT("");
        }

        switch (Friend->GetPresence().Status.State)
        {
        case EOnlinePresenceState::Away:
            return TEXT("Away");
        case EOnlinePresenceState::Chat:
            return TEXT("Chat");
        case EOnlinePresenceState::DoNotDisturb:
            return TEXT("Do Not Disturb");
        case EOnlinePresenceState::ExtendedAway:
            return TEXT("Extended Away");
        case EOnlinePresenceState::Offline:
            return TEXT("Offline");
        case EOnlinePresenceState::Online:
            return TEXT("Online");
        default:
            return TEXT("Unknown");
        }
    }

    UFUNCTION(BlueprintPure)
    FString GetPresenceStatusStatus() const
    {
        if (Friend == nullptr)
        {
            return TEXT("");
        }

        return Friend->GetPresence().Status.StatusStr;
    }

    UFUNCTION(BlueprintPure)
    bool HasVoiceSupport() const
    {
        if (Friend == nullptr)
        {
            return false;
        }

        return Friend->GetPresence().bHasVoiceSupport;
    }

    UFUNCTION(BlueprintPure)
    bool IsJoinable() const
    {
        if (Friend == nullptr)
        {
            return false;
        }

        return Friend->GetPresence().bIsJoinable;
    }

    UFUNCTION(BlueprintPure)
    bool IsPresenceOnline() const
    {
        if (Friend == nullptr)
        {
            return false;
        }

        return Friend->GetPresence().bIsOnline;
    }

    UFUNCTION(BlueprintPure)
    bool IsPresencePlayingThisGame() const
    {
        if (Friend == nullptr)
        {
            return false;
        }

        return Friend->GetPresence().bIsPlayingThisGame;
    }

    UFUNCTION(BlueprintPure)
    bool IsPresencePlaying() const
    {
        if (Friend == nullptr)
        {
            return false;
        }

        return Friend->GetPresence().bIsPlaying;
    }
};

USTRUCT(BlueprintType)
struct LEDESTUDIOSEOS_API FLSEOS_SimpleUser
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    FString UserId;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    FString DisplayName;
};