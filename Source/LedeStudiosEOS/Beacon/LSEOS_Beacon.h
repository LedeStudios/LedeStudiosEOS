// Copyright Lede Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconClient.h"
#include "OnlineBeaconHostObject.h"
#include "UObject/ObjectMacros.h"

#include "LSEOS_Beacon.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FLSEOS_BeaconPingComplete, bool, WasSuccessful, FString, Info);

UCLASS(transient, notplaceable)
class LEDESTUDIOSEOS_API ALSEOS_BeaconHost : public AOnlineBeaconHostObject
{
    GENERATED_UCLASS_BODY()

public:
    virtual AOnlineBeaconClient *SpawnBeaconActor(class UNetConnection *ClientConnection) override;
    virtual void OnClientConnected(class AOnlineBeaconClient *NewClientActor, class UNetConnection *ClientConnection)
        override;

    virtual bool Init();
};

UCLASS(transient, notplaceable)
class LEDESTUDIOSEOS_API ALSEOS_BeaconClient : public AOnlineBeaconClient
{
    GENERATED_BODY()

public:
    virtual void OnFailure() override;

    UPROPERTY()
    FLSEOS_BeaconPingComplete OnComplete;

    UFUNCTION(client, reliable)
    virtual void ClientSendPing();

    UFUNCTION(server, reliable)
    virtual void ServerRespondToPing();

    UFUNCTION(client, reliable)
    virtual void ClientReceivePong();
};

UCLASS(BlueprintType)
class LEDESTUDIOSEOS_API ALSEOS_BeaconHostActor : public AActor
{
    GENERATED_BODY()

private:
    UPROPERTY()
    AOnlineBeaconHost *Beacon;

public:
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;
};