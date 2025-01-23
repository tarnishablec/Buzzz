// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Item/BuzzzInstancingMode.h"
#include "BuzzzInstancingMode_PerItem.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Per Item")
class BUZZZ_API UBuzzzInstancingMode_PerItem : public UBuzzzInstancingMode
{
    GENERATED_BODY()

public:
    virtual UBuzzzInstance* MakeInstance_Implementation(const UBuzzzDefinition* Definition,
        TSubclassOf<UBuzzzInstance> InstanceClass,  AActor* Creator) const override;
};
