// Copyright Lede Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AchievementsTypes.generated.h"

USTRUCT(BlueprintType)
struct LEDESTUDIOSEOS_API FLSEOS_Achievement
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUnlocked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Progress = 0.0f;
};