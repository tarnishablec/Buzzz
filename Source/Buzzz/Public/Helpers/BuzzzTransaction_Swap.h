// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Transaction/BuzzzTransaction.h"
#include "BuzzzTransaction_Swap.generated.h"

struct FBuzzzTransaction_Common_Payload;
class UBuzzzContainer;
/**
 * 
 */
UCLASS(DisplayName = "Swap (Buzzz)")
class BUZZZ_API UBuzzzTransaction_Swap : public UBuzzzTransaction
{
    GENERATED_BODY()

    using FPayloadType = FBuzzzTransaction_Common_Payload;

    virtual void K2_OnExecute_Implementation() override;
};
