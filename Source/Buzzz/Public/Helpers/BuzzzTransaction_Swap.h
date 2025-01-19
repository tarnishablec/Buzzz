// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Transaction/BuzzzTransaction.h"
#include "BuzzzTransaction_Swap.generated.h"

struct FBuzzzTransactionPayload_Common;
class UBuzzzContainer;
/**
 * 
 */
UCLASS(DisplayName = "Swap (Buzzz)")
class BUZZZ_API UBuzzzTransaction_Swap : public UBuzzzTransaction
{
    GENERATED_BODY()

    using FPayloadType = FBuzzzTransactionPayload_Common;

    virtual void K2_OnExecute_Implementation() override;
};
