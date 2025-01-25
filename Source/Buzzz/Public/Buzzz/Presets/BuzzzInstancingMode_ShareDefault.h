// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Core/Item/BuzzzInstancingMode.h"
#include "BuzzzInstancingMode_ShareDefault.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Per Definition")
class BUZZZ_API UBuzzzInstancingMode_ShareDefault : public UBuzzzInstancingMode
{
    GENERATED_BODY()

public:
    virtual UBuzzzItem* MakeInstance_Implementation(
        TSubclassOf<UBuzzzItem> InstanceClass, AActor* Creator) const override;
};
