// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Transaction/BuzzzTransaction.h"
#include "Buzzz/Transaction/BuzzzBridgeBin.h"
#include "BuzzzTransaction_Recycle.generated.h"

class UBuzzzBridgeBin;

/**
 * 
 */
UCLASS(DisplayName = "Recycle (Buzzz)")
class BUZZZ_API UBuzzzTransaction_Recycle : public UBuzzzTransaction
{
    GENERATED_BODY()

    using FPayloadType = FBuzzzTransactionPayload_Common;

    virtual void K2_OnExecute_Implementation() override;
};
