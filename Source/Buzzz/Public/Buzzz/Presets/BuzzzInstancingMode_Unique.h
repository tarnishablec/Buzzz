// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Core/Item/BuzzzInstancingMode.h"
#include "BuzzzInstancingMode_Unique.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Per Item")
class BUZZZ_API UBuzzzInstancingMode_Unique : public UBuzzzInstancingMode
{
    GENERATED_BODY()

public:
    virtual UBuzzzItem* MakeInstance_Implementation(
        TSubclassOf<UBuzzzItem> InstanceClass,  AActor* Creator) const override;
};
