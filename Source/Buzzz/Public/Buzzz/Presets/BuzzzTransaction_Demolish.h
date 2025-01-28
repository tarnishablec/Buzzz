// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Transaction/BuzzzTransaction.h"
#include "BuzzzTransaction_Demolish.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Demolish (Buzzz)")
class BUZZZ_API UBuzzzTransaction_Demolish : public UBuzzzTransaction
{
    GENERATED_BODY()

    using FPayloadType = FBuzzzTransactionPayload_Common;

    virtual void K2_OnExecute_Implementation() override;
};
