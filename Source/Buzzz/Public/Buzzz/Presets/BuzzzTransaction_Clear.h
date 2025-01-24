// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Transaction/BuzzzTransaction.h"
#include "BuzzzTransaction_Clear.generated.h"

struct FBuzzzTransactionPayload_Common;
/**
 * 
 */
UCLASS(DisplayName = "Clear (Buzzz)")
class BUZZZ_API UBuzzzTransaction_Clear : public UBuzzzTransaction
{
    GENERATED_BODY()

    using FPayloadType = FBuzzzTransactionPayload_Common;

    virtual void K2_OnExecute_Implementation() override;
};
