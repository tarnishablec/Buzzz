// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Transaction/BuzzzTransaction.h"
#include "BuzzzTransaction_Kill.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Kill (Buzzz)")
class BUZZZ_API UBuzzzTransaction_Kill : public UBuzzzTransaction
{
    GENERATED_BODY()

    using FPayloadType = FBuzzzTransactionPayload_Common;

    virtual void K2_OnExecute_Implementation() override;
};
