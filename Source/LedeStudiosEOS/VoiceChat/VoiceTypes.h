// Copyright June Rhodes. MIT Licensed.

#pragma once
#include "VoiceChat.h"
#include "VoiceChatResult.h"

#include "VoiceTypes.generated.h"

UENUM(BlueprintType)
enum class ELSEOS_VoiceChatChannelType : uint8
{
    /** Non positional/2d audio channel */
    NonPositional,
    /** Positional/3d audio channel */
    Positional,
    /** Echo channel. Will only ever have one player and will echo anything you say */
    Echo
};

UENUM(BlueprintType)
enum class ELSEOS_VoiceChatAttenuationModel : uint8
{
    /** No attenuation is applied. The audio will drop to 0 at MaxDistance */
    None,
    /** The attenuation increases in inverse proportion to the distance. The Rolloff is the inverse of the slope of the
     * attenuation curve. */
    InverseByDistance,
    /** The attenuation increases in linear proportion to the distance. The Rolloff is the negative slope of the
     * attenuation curve. */
    LinearByDistance,
    /** The attenuation increases in inverse proportion to the distance raised to the power of the Rolloff. */
    ExponentialByDistance
};

UENUM(BlueprintType)
enum class ELSEOS_VoiceChatTransmitMode : uint8
{
    /** Transmit to none of the channels you are in */
    None,
    /** Transmit to all of the channels you are in */
    All,
    /** Transmit to a specific channel */
    Channel
};

UENUM(BlueprintType)
enum class ELSEOS_VoiceChatResult : uint8
{
    // The operation succeeded
    Success = 0,

    // Common state errors
    InvalidState,
    NotInitialized,
    NotConnected,
    NotLoggedIn,
    NotPermitted,
    Throttled,

    // Common argument errors
    InvalidArgument,
    CredentialsInvalid,
    CredentialsExpired,

    // Common connection errors
    ClientTimeout,
    ServerTimeout,
    DnsFailure,
    ConnectionFailure,

    // Error does not map to any common categories
    ImplementationError
};

/**
 * Blueprintable version of FVoiceChatChannel3dProperties
 */
USTRUCT(BlueprintType)
struct FLSEOS_VoiceChatChannel3dProperties
{

    GENERATED_BODY()

public:
    FLSEOS_VoiceChatChannel3dProperties()
        : AttenuationModel(ELSEOS_VoiceChatAttenuationModel::None), MinDistance(0.f), MaxDistance(0.f), Rolloff(0.f)
    {
    }

    FLSEOS_VoiceChatChannel3dProperties(const struct FVoiceChatChannel3dProperties &VoiceChatChannel3dProperties)
        : MinDistance(0.f), MaxDistance(0.f), Rolloff(0.f)
    {
        ConvertAttenuationModelToBlueprintableResult(VoiceChatChannel3dProperties.AttenuationModel, AttenuationModel);
    }

    /** The model used to determine how loud audio is at different distances */
    ELSEOS_VoiceChatAttenuationModel AttenuationModel;
    /** The distance at which the sound will start to attenuate */
    float MinDistance;
    /** The distance at which sound will no longer be audible */
    float MaxDistance;
    /** How fast the sound attenuates with distance */
    float Rolloff;

    static void ConvertAttenuationModelToBlueprintableResult(
        const EVoiceChatAttenuationModel &AttenuationModel, ELSEOS_VoiceChatAttenuationModel &OutAttenuationModel);

    static void ConvertBlueprintableAttenuationModelToAttenuationModel(
        const ELSEOS_VoiceChatAttenuationModel &AttenuationModel, EVoiceChatAttenuationModel &OutAttenuationModel);

    static void ConvertBlueprintableChannelTypeToChannelType(
        const ELSEOS_VoiceChatChannelType &ChannelType, EVoiceChatChannelType &OutChannelType);

    static FVoiceChatChannel3dProperties ConvertBlueprintableChatChannelPropertiesToChatChannelProperties(
        const FLSEOS_VoiceChatChannel3dProperties &ChatChannel3dProperties);
};

/**
 * Blueprintable version of FVoiceChatResult
 */
USTRUCT(BlueprintType)
struct FLSEOS_VoiceChatResult
{
    GENERATED_BODY()

public:
    FLSEOS_VoiceChatResult()
        : PlayerName(FString()), bWasSuccessful(false), VoiceChatResult(ELSEOS_VoiceChatResult::ImplementationError),
          ErrorCode(FString()), ErrorNum(0), ErrorDescription(FString())
    {
    }

    FLSEOS_VoiceChatResult(const FVoiceChatResult &Result)
        : PlayerName(FString()), bWasSuccessful(Result.IsSuccess()), ErrorCode(Result.ErrorCode),
          ErrorNum(Result.ErrorNum), ErrorDescription(Result.ErrorDesc)
    {
        ConvertVoiceChatResultToBlueprintableResult(Result.ResultCode, VoiceChatResult);
    }

    UPROPERTY(BlueprintReadOnly, Category = "Voice")
    FString PlayerName;

    UPROPERTY(BlueprintReadOnly, Category = "Voice")
    bool bWasSuccessful;

    UPROPERTY(BlueprintReadOnly, Category = "Voice")
    ELSEOS_VoiceChatResult VoiceChatResult;

    UPROPERTY(BlueprintReadOnly, Category = "Voice")
    FString ErrorCode;

    UPROPERTY(BlueprintReadOnly, Category = "Voice")
    int32 ErrorNum;

    UPROPERTY(BlueprintReadOnly, Category = "Voice")
    FString ErrorDescription;

    static void ConvertVoiceChatResultToBlueprintableResult(const EVoiceChatResult &VoiceChatResult, ELSEOS_VoiceChatResult &OutResult);
};