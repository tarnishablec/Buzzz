// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Transaction/BuzzzTransaction.h"
#include "BuzzzTransaction_Merge.generated.h"

struct FBuzzzTransactionPayload_Common;
class UBuzzzContainer;

/**
 * 
 */
UCLASS(DisplayName = "Merge (Buzzz)")
class BUZZZ_API UBuzzzTransaction_Merge : public UBuzzzTransaction
{
    GENERATED_BODY()

    using FPayloadType = FBuzzzTransactionPayload_Common;

    virtual void K2_OnExecute_Implementation() override;
};
