// Copyright Lede Studios. All Rights Reserved.

#pragma once

#include "TitleFileTypes.generated.h"

UCLASS(BlueprintType, Transient, Meta = (DontUseGenericSpawnObject, IsBlueprintBase = "false"))
class LEDESTUDIOSEOS_API ULSEOS_FileData : public UObject
{
    GENERATED_BODY()

private:
    TArray<uint8> Data;

public:
    static ULSEOS_FileData *FromNative(const TArray<uint8> &InData)
    {
        const auto Ref = NewObject<ULSEOS_FileData>();
        Ref->Data = InData;
        return Ref;
    }
    
    TArray<uint8> ToNative() const
    {
        return this->Data;
    }
    
    const TArray<uint8> &GetData() const
    {
        return this->Data;
    }
};