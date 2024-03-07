// Copyright Lede Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlinePartyInterface.h"
#include "PartiesTypes.generated.h"

UCLASS(BlueprintType)
class LEDESTUDIOSEOS_API ULSEOS_PartyId : public UObject
{
    GENERATED_BODY()

public:
    TSharedPtr<const FOnlinePartyId> PartyId;

    UFUNCTION(BlueprintCallable)
    FString ToString() const
    {
        if (PartyId == nullptr)
        {
            return TEXT("");
        }

        return PartyId->ToString();
    }
};

UCLASS(BlueprintType)
class LEDESTUDIOSEOS_API ULSEOS_PartyMemberId : public UObject
{
    GENERATED_BODY()

public:
    FOnlinePartyMemberConstPtr PartyMember;

    UFUNCTION(BlueprintCallable)
    FString ToString() const
    {
        if (PartyMember == nullptr)
        {
            return TEXT("");
        }

        return PartyMember->GetUserId()->ToString();
    }
};

UCLASS(BlueprintType)
class LEDESTUDIOSEOS_API ULSEOS_PartyInvite : public UObject
{
    GENERATED_BODY()

public:
    IOnlinePartyJoinInfoConstPtr PartyInvite;

    UFUNCTION(BlueprintCallable)
    FString ToString() const
    {
        if (PartyInvite == nullptr)
        {
            return TEXT("");
        }

        return PartyInvite->GetPartyId()->ToString();
    }
};