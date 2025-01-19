// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Transaction/BuzzzTransaction.h"
#include "BuzzzTransaction_Clear.generated.h"

struct FBuzzzTransaction_Common_Payload;
/**
 * 
 */
UCLASS(DisplayName = "Clear (Buzzz)")
class BUZZZ_API UBuzzzTransaction_Clear : public UBuzzzTransaction
{
    GENERATED_BODY()

    using FPayloadType = FBuzzzTransaction_Common_Payload;

    virtual void K2_OnExecute_Implementation() override;
};
