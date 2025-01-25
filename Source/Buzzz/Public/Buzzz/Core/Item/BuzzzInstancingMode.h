// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BuzzzInstancingMode.generated.h"

class UBuzzzItem;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BUZZZ_API UBuzzzInstancingMode : public UObject
{
    GENERATED_BODY()

public:
  

    UFUNCTION(BlueprintNativeEvent, Category="Buzzz")
    UBuzzzItem* MakeInstance(
        TSubclassOf<UBuzzzItem> InstanceClass, AActor* Creator) const;
};
