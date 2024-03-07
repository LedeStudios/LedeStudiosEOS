// Copyright Lede Studios. All Rights Reserved.

#include "LSEOS_Beacon.h"

#include "OnlineBeaconHost.h"

#include "CoreGlobals.h"
#include "Config/LSEOS_ConfigLibrary.h"
#include "Misc/ConfigCacheIni.h"

ALSEOS_BeaconHost::ALSEOS_BeaconHost(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    ClientBeaconActorClass = ALSEOS_BeaconClient::StaticClass();
    BeaconTypeName = ClientBeaconActorClass->GetName();
}

bool ALSEOS_BeaconHost::Init()
{
    UE_LOG(LogTemp, Verbose, TEXT("Server beacon was initialized"));
    return true;
}

void ALSEOS_BeaconHost::OnClientConnected(AOnlineBeaconClient *NewClientActor, UNetConnection *ClientConnection)
{
    Super::OnClientConnected(NewClientActor, ClientConnection);

    ALSEOS_BeaconClient *BeaconClient = Cast<ALSEOS_BeaconClient>(NewClientActor);
    if (BeaconClient != nullptr)
    {
        BeaconClient->ClientSendPing();
    }
}

AOnlineBeaconClient *ALSEOS_BeaconHost::SpawnBeaconActor(UNetConnection *ClientConnection)
{
    return Super::SpawnBeaconActor(ClientConnection);
}

void ALSEOS_BeaconClient::OnFailure()
{
    UE_LOG(LogTemp, Error, TEXT("Demo beacon connection failure; please check the logs."));

    Super::OnFailure();
}

void ALSEOS_BeaconClient::ClientSendPing_Implementation()
{
    UE_LOG(LogTemp, Verbose, TEXT("Pinging server through beacon..."));
    this->ServerRespondToPing();
}

void ALSEOS_BeaconClient::ServerRespondToPing_Implementation()
{
    UE_LOG(LogTemp, Verbose, TEXT("Server responding to beacon ping..."));
    this->ClientReceivePong();
}

void ALSEOS_BeaconClient::ClientReceivePong_Implementation()
{
    UE_LOG(LogTemp, Verbose, TEXT("Client got server response."));
    // ReSharper disable once CppExpressionWithoutSideEffects
    this->OnComplete.ExecuteIfBound(true, TEXT("Beacon ping successful"));
    this->DestroyBeacon();
}

void ALSEOS_BeaconHostActor::BeginPlay()
{
    if (!ULSEOS_ConfigLibrary::GetAreBeaconsSupported(this))
    {
        // Beacons are not supported on the legacy networking stack.
        return;
    }

    if (Beacon)
    {
        Beacon->DestroyBeacon();
        Beacon = nullptr;
    }

    AOnlineBeaconHost *BeaconHost = GetWorld()->SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass());

    // If you explicitly set the port number here, then you should pass the port number
    // as e.g. FName(TEXT("12345")) as the port name when calling GetResolvedConnectString.
    //
    // If you *don't* set the port number here, and leave it as the default, then you should
    // pass NAME_BeaconPort as the port name when calling GetResolvedConnectString.
    BeaconHost->ListenPort = 12345;

    if (BeaconHost && BeaconHost->InitHost())
    {
        if (AOnlineBeaconHostObject *BeaconHostObject =
            GetWorld()->SpawnActor<AOnlineBeaconHostObject>(ALSEOS_BeaconHost::StaticClass()))
        {
            BeaconHost->RegisterHost(BeaconHostObject);
            Beacon = BeaconHost;
        }
        BeaconHost->PauseBeaconRequests(false);
    }
}

void ALSEOS_BeaconHostActor::BeginDestroy()
{
    if (Beacon)
    {
        Beacon->DestroyBeacon();
        Beacon = nullptr;
    }

    Super::BeginDestroy();
}