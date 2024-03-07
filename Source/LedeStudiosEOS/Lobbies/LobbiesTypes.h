// Copyright Lede Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineLobbyInterface.h"
#include "LobbiesTypes.generated.h"

UCLASS(BlueprintType)
class LEDESTUDIOSEOS_API ULSEOS_LobbyId : public UObject
{
    GENERATED_BODY()

private:
    TSharedPtr<const FOnlineLobbyId> LobbyId;

public:
    static ULSEOS_LobbyId *FromNative(const FOnlineLobbyId &InObj)
    {
        const auto Ref = NewObject<ULSEOS_LobbyId>();
        Ref->LobbyId = TSharedPtr<const FOnlineLobbyId>(InObj.AsShared());
        return Ref;
    }
    
    static ULSEOS_LobbyId *FromNative(const TSharedPtr<const FOnlineLobbyId> &InObj)
    {
        const auto Ref = NewObject<ULSEOS_LobbyId>();
        Ref->LobbyId = InObj;
        return Ref;
    }
    
    const FOnlineLobbyId &ToNative() const
    {
        return this->LobbyId.ToSharedRef().Get();
    }
    
    bool IsValid() const
    {
        return this->LobbyId.IsValid();
    }

    UFUNCTION(BlueprintPure, Category = "Online")
    FString ToDebugString() const
    {
        if (!this->IsValid())
        {
            return TEXT("");
        }

        return this->LobbyId->ToDebugString();
    }
};

UCLASS(BlueprintType, Transient, Meta = (DontUseGenericSpawnObject, IsBlueprintBase = "false"))
class LEDESTUDIOSEOS_API ULSEOS_Lobby : public UObject
{
    GENERATED_BODY()

private:
    TSharedPtr<FOnlineLobby> Lobby;

public:
    UFUNCTION(BlueprintPure, Category = "Online")
    ULSEOS_LobbyId *GetId() const
    {
        if (!this->Lobby.IsValid())
        {
            return nullptr;
        }

        return ULSEOS_LobbyId::FromNative(this->Lobby->Id);
    }

    UFUNCTION(BlueprintPure, Category = "Online")
    FUniqueNetIdRepl GetOwnerId() const
    {
        if (!this->Lobby.IsValid())
        {
            return FUniqueNetIdRepl();
        }

        return Lobby->OwnerId;
    }

    static ULSEOS_Lobby *FromNative(TSharedPtr<FOnlineLobby> InObj)
    {
        const auto Ref = NewObject<ULSEOS_Lobby>();
        Ref->Lobby = TSharedPtr<FOnlineLobby>(InObj);
        return Ref;
    }
    
    TSharedPtr<FOnlineLobby> ToNative() const
    {
        return this->Lobby;
    }
    
    bool IsValid() const
    {
        return this->Lobby.IsValid();
    }
};

UCLASS(BlueprintType, Transient, Meta = (DontUseGenericSpawnObject, IsBlueprintBase = "false"))
class LEDESTUDIOSEOS_API ULSEOS_OnlineLobbyTransaction : public UObject
{
    GENERATED_BODY()

private:
    TSharedPtr<FOnlineLobbyTransaction> Txn;

public:
    void SetMetadata(const FString &Key, const FVariantData &Value) const
    {
        Txn->SetMetadata.Add(Key, Value);
    }

    void SetMetadataByMap(const TMap<FString, FVariantData> &Metadata) const
    {
        for (auto &&It : Metadata)
        {
            Txn->SetMetadata.Add(It.Key, It.Value);
        }
    }

    void DeleteMetadata(const FString &Key) const
    {
        Txn->DeleteMetadata.Remove(Key);
    }

    void DeleteMetadataByArray(const TArray<FString> &MetadataKeys) const
    {
        for (auto &&It : MetadataKeys)
        {
            Txn->DeleteMetadata.Remove(It);
        }
    }

    void SetLocked(const bool Locked) const
    {
        Txn->Locked = Locked;
    }

    void SetCapacity(const int64 Capacity) const
    {
        Txn->Capacity = Capacity;
    }

    void SetPublic(const bool Public) const
    {
        Txn->Public = Public;
    }

    static ULSEOS_OnlineLobbyTransaction *FromNative(const TSharedPtr<FOnlineLobbyTransaction>& InObj)
    {
        const auto Ref = NewObject<ULSEOS_OnlineLobbyTransaction>();
        Ref->Txn = InObj;
        return Ref;
    }
    
    TSharedPtr<FOnlineLobbyTransaction> ToNative() const
    {
        return this->Txn;
    }
    
    bool IsValid() const
    {
        return this->Txn.IsValid();
    }
};

UCLASS(BlueprintType, Transient, Meta = (DontUseGenericSpawnObject, IsBlueprintBase = "false"))
class LEDESTUDIOSEOS_API ULSEOS_OnlineLobbyMemberTransaction : public UObject
{
    GENERATED_BODY()

private:
    TSharedPtr<FOnlineLobbyMemberTransaction> Txn;

public:
    void SetMetadata(const FString &Key, const FVariantData &Value) const
    {
        Txn->SetMetadata.Add(Key, Value);
    }

    void SetMetadataByMap(const TMap<FString, FVariantData> &Metadata) const
    {
        for (auto &&It : Metadata)
        {
            Txn->SetMetadata.Add(It.Key, It.Value);
        }
    }

    void DeleteMetadata(const FString &Key) const
    {
        Txn->DeleteMetadata.Remove(Key);
    }

    void DeleteMetadataByArray(const TArray<FString> &MetadataKeys) const
    {
        for (auto &&It : MetadataKeys)
        {
            Txn->DeleteMetadata.Remove(It);
        }
    }

    static ULSEOS_OnlineLobbyMemberTransaction *FromNative(const TSharedPtr<FOnlineLobbyMemberTransaction>& InObj)
    {
        const auto Ref = NewObject<ULSEOS_OnlineLobbyMemberTransaction>();
        Ref->Txn = InObj;
        return Ref;
    }
    
    TSharedPtr<FOnlineLobbyMemberTransaction> ToNative() const
    {
        return this->Txn;
    }
    
    bool IsValid() const
    {
        return this->Txn.IsValid();
    }
};

USTRUCT(BlueprintType)
struct FLSEOS_CreateOrConnectLobbyResult
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorRaw;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorCode;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FText ErrorMessage;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FUniqueNetIdRepl UserId;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    ULSEOS_Lobby* Lobby = nullptr;
};

USTRUCT(BlueprintType)
struct FLSEOS_LobbySearchResult
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorRaw;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorCode;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FText ErrorMessage;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FUniqueNetIdRepl UserId;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    TArray<ULSEOS_LobbyId*> Lobbies;
};

USTRUCT(BlueprintType)
struct FLSEOS_UpdateLobbyResult
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorRaw;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorCode;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FText ErrorMessage;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FUniqueNetIdRepl UserId;
};