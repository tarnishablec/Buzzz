// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "BuzzzTransaction.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class BUZZZ_API UBuzzzTransaction : public UObject
{
    GENERATED_BODY()

public:
    UBuzzzTransaction()
    {
        TransactionID = FGuid::NewGuid();
    }

    UPROPERTY()
    FGuid TransactionID;

    UPROPERTY(BlueprintReadOnly, Category="Buzzz")
    FInstancedStruct Payload;

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category="Buzzz")
    void OnExecute();

    virtual void Rollback();
};
