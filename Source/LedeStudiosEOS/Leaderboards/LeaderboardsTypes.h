// Copyright Lede Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LeaderboardsTypes.generated.h"

USTRUCT(BlueprintType)
struct LEDESTUDIOSEOS_API FLSEOS_LeaderboardEntry
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int Rank = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString PlayerId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString PlayerNickname;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int Score = 0;
};