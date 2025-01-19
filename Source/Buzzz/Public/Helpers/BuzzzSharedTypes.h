// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BuzzzSharedTypes.generated.h"

class UBuzzzContainer;

USTRUCT(BlueprintType)
struct FBuzzzTransactionPayload_Common
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category="Buzzz")
    TObjectPtr<UBuzzzContainer> TargetContainer;

    UPROPERTY(BlueprintReadWrite, Category="Buzzz")
    int32 TargetIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadWrite, Category="Buzzz")
    TObjectPtr<UBuzzzContainer> FromContainer;

    UPROPERTY(BlueprintReadWrite, Category="Buzzz")
    int32 FromIndex = INDEX_NONE;
};
