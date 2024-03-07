// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Parties/PartiesTypes.h"

void ULS_EOSSubsystem::CreateLobby(const int64& ExampleAttributeData, FLSEOS_OnCreateOrConnectLobbyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    FLSEOS_CreateOrConnectLobbyResult ExampleCPPCreateOrConnectLobbyResult;

    const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get identity interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("UniqueNetId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    TSharedPtr<IOnlineLobby, ESPMode::ThreadSafe> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get lobby interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    //TODO: Update Example Values
    FVariantData ExampleVariantData = FVariantData();
    ExampleVariantData.SetValue(ExampleAttributeData);

    const TSharedPtr<FOnlineLobbyTransaction> OnlineLobbyTransaction =
        Lobby->MakeCreateLobbyTransaction(*Identity->GetUniquePlayerId(0));

    //TODO: Update Example Values
    OnlineLobbyTransaction->SetMetadata.Add("ExampleAttributeData", ExampleVariantData);
    OnlineLobbyTransaction->SetMetadata.Add("SearchValue", true);
    OnlineLobbyTransaction->Public = true;
    OnlineLobbyTransaction->Locked = false;

    FOnLobbyCreateOrConnectComplete OnLobbyCreateOrConnectComplete;
    OnLobbyCreateOrConnectComplete.BindUObject(this, &ULS_EOSSubsystem::HandleCreateLobbyComplete, OnDone);

    if (!Lobby->CreateLobby(*Identity->GetUniquePlayerId(0), *OnlineLobbyTransaction, OnLobbyCreateOrConnectComplete))
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("Lobby creation exited early.");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
    }
}

void ULS_EOSSubsystem::HandleCreateLobbyComplete(const FOnlineError& Error, const FUniqueNetId& UserId,
	const TSharedPtr<FOnlineLobby>& Lobby, FLSEOS_OnCreateOrConnectLobbyComplete OnDone)
{
    FLSEOS_CreateOrConnectLobbyResult CreateOrConnectLobbyResult;
    CreateOrConnectLobbyResult.ErrorRaw = Error.ErrorRaw;
    CreateOrConnectLobbyResult.ErrorCode = Error.ErrorCode;
    CreateOrConnectLobbyResult.ErrorMessage = Error.ErrorMessage;
    CreateOrConnectLobbyResult.UserId = UserId.AsShared();
    CreateOrConnectLobbyResult.Lobby = ULSEOS_Lobby::FromNative(Lobby);

    OnDone.ExecuteIfBound(CreateOrConnectLobbyResult, Error.bSucceeded);
}

void ULS_EOSSubsystem::ConnectLobby(ULSEOS_LobbyId* LobbyId, FLSEOS_OnCreateOrConnectLobbyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    FLSEOS_CreateOrConnectLobbyResult ExampleCPPCreateOrConnectLobbyResult;

    if (LobbyId == nullptr)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("LobbyId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get identity interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("UniqueNetId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    const TSharedPtr<IOnlineLobby> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get lobby interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    const TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    FOnLobbyCreateOrConnectComplete OnLobbyCreateOrConnectComplete;
    OnLobbyCreateOrConnectComplete.BindUObject(this, &ULS_EOSSubsystem::HandleConnectLobbyComplete, OnDone);

    if (!Lobby->ConnectLobby(*Identity->GetUniquePlayerId(0), *OnlineLobbyId, OnLobbyCreateOrConnectComplete))
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("Lobby connect exited early.");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
    }
}

void ULS_EOSSubsystem::HandleConnectLobbyComplete(const FOnlineError& Error, const FUniqueNetId& UserId,
	const TSharedPtr<FOnlineLobby>& Lobby, FLSEOS_OnCreateOrConnectLobbyComplete OnDone)
{
    FLSEOS_CreateOrConnectLobbyResult CreateOrConnectLobbyResult;
    CreateOrConnectLobbyResult.ErrorRaw = Error.ErrorRaw;
    CreateOrConnectLobbyResult.ErrorCode = Error.ErrorCode;
    CreateOrConnectLobbyResult.ErrorMessage = Error.ErrorMessage;
    CreateOrConnectLobbyResult.UserId = UserId.AsShared();
    CreateOrConnectLobbyResult.Lobby = ULSEOS_Lobby::FromNative(Lobby);

    OnDone.ExecuteIfBound(CreateOrConnectLobbyResult, Error.bSucceeded);
}

void ULS_EOSSubsystem::DisconnectLobby(ULSEOS_LobbyId* LobbyId, FLSEOS_OnCreateOrConnectLobbyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    FLSEOS_CreateOrConnectLobbyResult ExampleCPPCreateOrConnectLobbyResult;

    if (LobbyId == nullptr)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("LobbyId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get identity interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("UniqueNetId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    const TSharedPtr<IOnlineLobby> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get lobby interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    const TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    if (OnlineLobbyId.IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to create native lobby id from passed in lobby id");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    FOnLobbyOperationComplete OnLobbyOperationComplete;
    OnLobbyOperationComplete.BindUObject(this, &ULS_EOSSubsystem::HandleDisconnectLobbyComplete, OnDone);

    if (!Lobby->DisconnectLobby(*Identity->GetUniquePlayerId(0), *OnlineLobbyId, OnLobbyOperationComplete))
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("Lobby disconnect exited early.");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
    }
}

void ULS_EOSSubsystem::HandleDisconnectLobbyComplete(const FOnlineError& Error, const FUniqueNetId& UserId,
	FLSEOS_OnCreateOrConnectLobbyComplete OnDone)
{
    FLSEOS_CreateOrConnectLobbyResult CreateOrConnectLobbyResult;
    CreateOrConnectLobbyResult.ErrorRaw = Error.ErrorRaw;
    CreateOrConnectLobbyResult.ErrorCode = Error.ErrorCode;
    CreateOrConnectLobbyResult.ErrorMessage = Error.ErrorMessage;
    CreateOrConnectLobbyResult.UserId = UserId.AsShared();

    OnDone.ExecuteIfBound(CreateOrConnectLobbyResult, Error.bSucceeded);
}

void ULS_EOSSubsystem::SearchLobby(FLSEOS_OnSearchLobbyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    FLSEOS_LobbySearchResult ExampleCPPLobbySearchResult;

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        ExampleCPPLobbySearchResult.ErrorMessage = FText::FromString("Unable to get identity interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPLobbySearchResult, false);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        ExampleCPPLobbySearchResult.ErrorMessage = FText::FromString("UniqueNetId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPLobbySearchResult, false);
        return;
    }

    TSharedPtr<IOnlineLobby, ESPMode::ThreadSafe> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        ExampleCPPLobbySearchResult.ErrorMessage = FText::FromString("Unable to get lobby interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPLobbySearchResult, false);
        return;
    }

    FOnLobbySearchComplete OnLobbySearchComplete;
    OnLobbySearchComplete.BindUObject(this, &ULS_EOSSubsystem::HandleSearchLobbyComplete, OnDone);
    
    //TODO: Update Example Values
    FOnlineLobbySearchQueryFilter OnlineLobbySearchQueryFilter =
        FOnlineLobbySearchQueryFilter("SearchValue", true, EOnlineLobbySearchQueryFilterComparator::Equal);

    FOnlineLobbySearchQuery OnlineLobbySearchQuery;
    OnlineLobbySearchQuery.Filters.Add(OnlineLobbySearchQueryFilter);

    if (!Lobby->Search(*Identity->GetUniquePlayerId(0), OnlineLobbySearchQuery, OnLobbySearchComplete))
    {
        ExampleCPPLobbySearchResult.ErrorMessage = FText::FromString("Lobby search exited early.");
        OnDone.ExecuteIfBound(ExampleCPPLobbySearchResult, false);
    }
}

void ULS_EOSSubsystem::HandleSearchLobbyComplete(const FOnlineError& Error, const FUniqueNetId& UserId,
	const TArray<TSharedRef<const FOnlineLobbyId>>& LobbyIds, FLSEOS_OnSearchLobbyComplete OnDone)
{
    FLSEOS_LobbySearchResult LobbySearchResult;
    LobbySearchResult.ErrorRaw = Error.ErrorRaw;
    LobbySearchResult.ErrorCode = Error.ErrorCode;
    LobbySearchResult.ErrorMessage = Error.ErrorMessage;
    LobbySearchResult.UserId = UserId.AsShared();

    TArray<ULSEOS_LobbyId*> Lobbies;
    Lobbies.Reserve(LobbyIds.Num());

    for (auto &&LobbyId : LobbyIds)
    {
        ULSEOS_LobbyId* ExampleLobbyId = ULSEOS_LobbyId::FromNative(LobbyId);
        Lobbies.Add(ExampleLobbyId);
    }

    LobbySearchResult.Lobbies = Lobbies;
    OnDone.ExecuteIfBound(LobbySearchResult, Error.bSucceeded);
}

void ULS_EOSSubsystem::UpdateLobby(ULSEOS_LobbyId* LobbyId, const int64& ExampleAttributeData,
	FLSEOS_OnUpdateLobbyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    FLSEOS_UpdateLobbyResult ExampleCPPUpdateLobbyResult;

    if (LobbyId == nullptr)
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage = FText::FromString("LobbyId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
        return;
    }

    const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage = FText::FromString("Unable to get identity interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage = FText::FromString("UniqueNetId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
        return;
    }

    const TSharedPtr<IOnlineLobby> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage = FText::FromString("Unable to get lobby interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
        return;
    }

    const TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    if (OnlineLobbyId.IsValid() == false)
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage =
            FText::FromString("Unable to create native lobby id from passed in lobby id");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
        return;
    }

    //TODO: Update Example Values
    FVariantData ExampleVariantData = FVariantData();
    ExampleVariantData.SetValue(ExampleAttributeData);

    const TSharedPtr<FOnlineLobbyTransaction> OnlineLobbyTransaction =
        Lobby->MakeCreateLobbyTransaction(*Identity->GetUniquePlayerId(0));
    
    //TODO: Update Example Values
    OnlineLobbyTransaction->SetMetadata.Add("ExampleAttributeData", ExampleVariantData);

    FOnLobbyOperationComplete OnLobbyOperationComplete;
    OnLobbyOperationComplete.BindUObject(this, &ULS_EOSSubsystem::HandleUpdateLobbyComplete, OnDone);

    if (!Lobby->UpdateLobby(*Identity->GetUniquePlayerId(0),
        *OnlineLobbyId, *OnlineLobbyTransaction, OnLobbyOperationComplete))
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage = FText::FromString("Lobby update exited early.");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
    }
}

void ULS_EOSSubsystem::HandleUpdateLobbyComplete(const FOnlineError& Error, const FUniqueNetId& UserId,
	FLSEOS_OnUpdateLobbyComplete OnDone)
{
    FLSEOS_UpdateLobbyResult UpdateLobbyResult;
    UpdateLobbyResult.ErrorRaw = Error.ErrorRaw;
    UpdateLobbyResult.ErrorCode = Error.ErrorCode;
    UpdateLobbyResult.ErrorMessage = Error.ErrorMessage;
    UpdateLobbyResult.UserId = UserId.AsShared();

    OnDone.ExecuteIfBound(UpdateLobbyResult, Error.bSucceeded);
}

bool ULS_EOSSubsystem::GetMemberCount(ULSEOS_LobbyId* LobbyId, int32& OutMemberCount)
{
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    if (LobbyId == nullptr)
    {
        return false;
    }

    const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        return false;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        return false;
    }

    const TSharedPtr<IOnlineLobby> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        return false;
    }

    const TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    if (OnlineLobbyId.IsValid() == false)
    {
        return false;
    }

    return Lobby->GetMemberCount(*Identity->GetUniquePlayerId(0), *OnlineLobbyId, OutMemberCount);
}

bool ULS_EOSSubsystem::GetMemberUserId(ULSEOS_LobbyId* LobbyId, const int32& MemberIndex, FString& OutMemberId)
{
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    if (LobbyId == nullptr)
    {
        return false;
    }

    const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        return false;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        return false;
    }

    const TSharedPtr<IOnlineLobby> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        return false;
    }

    const TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    if (OnlineLobbyId.IsValid() == false)
    {
        return false;
    }

    int32 MemberCount;
    const bool bWasSuccessful = Lobby->GetMemberCount(*Identity->GetUniquePlayerId(0), *OnlineLobbyId, MemberCount);

    if (bWasSuccessful == false || MemberCount <= 0)
    {
        return false;
    }

    TSharedPtr<const FUniqueNetId> MemberId;
    const bool bFoundMember =
        Lobby->GetMemberUserId(*Identity->GetUniquePlayerId(0), *OnlineLobbyId, MemberIndex, MemberId);

    if (MemberId.IsValid() == false)
    {
        return false;
    }

    OutMemberId = MemberId->ToString();
    return bFoundMember;
}

ULSEOS_PartyInvite* ULS_EOSSubsystem::ConvertLobbyIdToPartyJoinInfo(ULSEOS_LobbyId* LobbyId)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    if (LobbyId == nullptr)
    {
        return nullptr;
    }

    const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        return nullptr;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        return nullptr;
    }

    const TSharedPtr<IOnlineLobby> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        return nullptr;
    }

    const TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    if (OnlineLobbyId.IsValid() == false)
    {
        return nullptr;
    }

    const IOnlinePartyPtr Party = Subsystem->GetPartyInterface();
    if (Party == nullptr)
    {
        return nullptr;
    }

    const FString JoinInfoJson = Party->MakeJoinInfoJson(*Identity->GetUniquePlayerId(0), *OnlineLobbyId);

    if (JoinInfoJson.IsEmpty())
    {
        return nullptr;
    }

    const IOnlinePartyJoinInfoConstPtr JoinInfoPtr = Party->MakeJoinInfoFromJson(JoinInfoJson);
    if (!JoinInfoPtr.IsValid())
    {
        return nullptr;
    }

    ULSEOS_PartyInvite* PartyInvite = NewObject<ULSEOS_PartyInvite>(this);
    PartyInvite->PartyInvite = JoinInfoPtr;

    return PartyInvite;
}










