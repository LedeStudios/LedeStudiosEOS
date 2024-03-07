// Copyright Lede Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StatsTypes.generated.h"

USTRUCT(BlueprintType)
struct LEDESTUDIOSEOS_API FLSEOS_Stat
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString Id;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int CurrentValue = 0;
};