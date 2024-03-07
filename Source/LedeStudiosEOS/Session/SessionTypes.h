// Copyright Lede Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "SessionTypes.generated.h"

UCLASS(BlueprintType)
class LEDESTUDIOSEOS_API ULSEOS_SessionSearchResult : public UObject
{
    GENERATED_BODY()

public:
    FOnlineSessionSearchResult Result;

    UPROPERTY(BlueprintReadOnly)
    FString SessionId;

    UPROPERTY(BlueprintReadOnly)
    FString ConnectionString;

    UPROPERTY(BlueprintReadonly)
    bool bIsDedicatedServer;

    UPROPERTY(BlueprintReadOnly)
    int64 OpenSlots;

    UPROPERTY(BlueprintReadOnly)
    int64 TotalSlots;
};