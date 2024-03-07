// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "GameFramework/GameModeBase.h"

void ULS_EOSSubsystem::SeamlessTravel(const UObject* WorldContextObject, const FString& URL)
{
	AGameModeBase *GameModeBase = WorldContextObject->GetWorld()->GetAuthGameMode<AGameModeBase>();
	if (GameModeBase == nullptr)
	{
		return;
	}

	GameModeBase->bUseSeamlessTravel = true;
	WorldContextObject->GetWorld()->ServerTravel(URL, true);
}

void ULS_EOSSubsystem::NonSeamlessTravel(const UObject* WorldContextObject, const FString& URL)
{
	AGameModeBase *GameModeBase = WorldContextObject->GetWorld()->GetAuthGameMode<AGameModeBase>();
	if (GameModeBase == nullptr)
	{
		return;
	}

	GameModeBase->bUseSeamlessTravel = false;
	WorldContextObject->GetWorld()->ServerTravel(URL, true);
}

void ULS_EOSSubsystem::BeginRecordingReplay(AGameModeBase* GameMode)
{
	const ENetMode NetMode = GameMode->GetWorld()->GetNetMode();
	const bool bIsServer = (NetMode == NM_DedicatedServer || NetMode == NM_ListenServer);
	if (GameMode->GetWorld()->IsGameWorld() && bIsServer)
	{
		GameMode->GetGameInstance()->StartRecordingReplay(FString("Replay"), FString("Replay"));
	}
}


