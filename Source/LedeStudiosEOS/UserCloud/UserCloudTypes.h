// Copyright Lede Studios. All Rights Reserved.


#pragma once

#include "UserCloudTypes.generated.h"

USTRUCT(BlueprintType)
struct LEDESTUDIOSEOS_API FLSEOS_CloudFileData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    FString Hash;

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    FName HashType;

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    FString DLName;

    UPROPERTY(BlueprintReadWrite, Category = "User Cloud")
    FString FileName;

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    int32 FileSize = 0;

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    FString URL;

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    int64 ChunkID = 0;

    static FLSEOS_CloudFileData FromNative(const FCloudFileHeader& FileHeader)
    {
        FLSEOS_CloudFileData Result;
        Result.Hash = FileHeader.Hash;
        Result.HashType = FileHeader.HashType;
        Result.DLName = FileHeader.DLName;
        Result.FileName = FileHeader.FileName;
        Result.FileSize = FileHeader.FileSize;
        Result.URL = FileHeader.URL;
        Result.ChunkID = FileHeader.ChunkID;
        return Result;
    }
};