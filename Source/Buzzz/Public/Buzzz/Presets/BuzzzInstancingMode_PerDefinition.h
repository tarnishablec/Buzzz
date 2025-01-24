// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Core/Item/BuzzzInstancingMode.h"
#include "BuzzzInstancingMode_PerDefinition.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Per Definition")
class BUZZZ_API UBuzzzInstancingMode_PerDefinition : public UBuzzzInstancingMode
{
    GENERATED_BODY()

public:
    virtual UBuzzzInstance* MakeInstance_Implementation(const UBuzzzDefinition* Definition,
        TSubclassOf<UBuzzzInstance> InstanceClass,  AActor* Creator) const override;
};
