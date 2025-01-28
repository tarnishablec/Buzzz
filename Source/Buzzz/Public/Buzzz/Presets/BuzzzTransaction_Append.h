// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Transaction/BuzzzTransaction.h"
#include "BuzzzTransaction_Append.generated.h"

USTRUCT(BlueprintType)
struct BUZZZ_API FBuzzzTransactionPayload_Append : public FBuzzzTransactionPayload_Common
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    bool bAutoExpand = false;
};

/**
 * 
 */
UCLASS(DisplayName = "Append (Buzzz)")
class BUZZZ_API UBuzzzTransaction_Append : public UBuzzzTransaction
{
    GENERATED_BODY()

    using FPayloadType = FBuzzzTransactionPayload_Append;

    virtual void K2_OnExecute_Implementation() override;
};

