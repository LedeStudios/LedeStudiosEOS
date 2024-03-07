// Copyright Lede Studios. All Rights Reserved.

#include "LSEOS_ConfigLibrary.h"

bool ULSEOS_ConfigLibrary::GetPreferBlueprintsImplementation()
{
    return GetDefault<ULSEOS_Config>()->bPreferBlueprintsImplementation;
}

bool ULSEOS_ConfigLibrary::GetIsSteamEnabled(const UObject *WorldContextObject)
{
    bool Value = true;
    GConfig->GetBool(TEXT("OnlineSubsystemSteam"), TEXT("bEnabled"), Value, GEngineIni);
    return Value;
}

void ULSEOS_ConfigLibrary::SetIsSteamEnabled(const UObject *WorldContextObject, const bool EnableSteam)
{
    GConfig->SetBool(TEXT("OnlineSubsystemSteam"), TEXT("bEnabled"), EnableSteam, GEngineIni);
    GConfig->Flush(false, GEngineIni);
}

FString ULSEOS_ConfigLibrary::GetSteamAppId(const UObject *WorldContextObject)
{
    FString Value = TEXT("");
    GConfig->GetString(TEXT("OnlineSubsystemSteam"), TEXT("SteamDevAppId"), Value, GEngineIni);
    return Value;
}

void ULSEOS_ConfigLibrary::SetSteamAppId(const UObject *WorldContextObject, const FString InAppId)
{
    GConfig->SetString(TEXT("OnlineSubsystemSteam"), TEXT("SteamDevAppId"), *InAppId, GEngineIni);
    GConfig->Flush(false, GEngineIni);
}

TArray<FString> ULSEOS_ConfigLibrary::GetDelegatedSubsystems(const UObject *WorldContextObject)
{
    FString Value = TEXT("");
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("DelegatedSubsystems"), Value, GEngineIni);

    TArray<FString> Results;
    Value.ParseIntoArray(Results, TEXT(","), true);

    return Results;
}

void ULSEOS_ConfigLibrary::SetDelegatedSubsystems(
    const UObject *WorldContextObject,
    const TArray<FString> InSubsystemNames)
{
    const FString NewValue = FString::Join(InSubsystemNames, TEXT(","));
    GConfig->SetString(TEXT("EpicOnlineServices"), TEXT("DelegatedSubsystems"), *NewValue, GEngineIni);
    GConfig->Flush(false, GEngineIni);
}

bool ULSEOS_ConfigLibrary::GetAreBeaconsSupported(const UObject *WorldContextObject)
{
    FString Value = TEXT("");
    FString ApiVersion = TEXT("");
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("NetworkingStack"), Value, GEngineIni);
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("ApiVersion"), ApiVersion, GEngineIni);
    if (Value == TEXT("Full"))
    {
        return true;
    }
    else if (Value == TEXT("Legacy"))
    {
        return false;
    }
    else
    {
        return !(
            ApiVersion == TEXT("v2020_12_09") || ApiVersion == TEXT("v2020_12_02") ||
            ApiVersion == TEXT("v2020_12_01"));
    }
}