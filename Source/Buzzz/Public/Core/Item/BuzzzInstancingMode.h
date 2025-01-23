// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BuzzzInstancingMode.generated.h"

class UBuzzzDefinition;
class UBuzzzInstance;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BUZZZ_API UBuzzzInstancingMode : public UObject
{
    GENERATED_BODY()

public:
    UBuzzzInstancingMode()
    {
    }


    UFUNCTION(BlueprintPure, Category = "Buzzz", DisplayName="MakeInstance", meta=(DeterminesOutputType="InstanceClass"))
    static UBuzzzInstance* MakeInstance_Static(const UBuzzzDefinition* Definition,
                                               TSubclassOf<UBuzzzInstance> InstanceClass,
                                               AActor* Creator);

    UFUNCTION(BlueprintNativeEvent, Category="Buzzz")
    UBuzzzInstance* MakeInstance(const UBuzzzDefinition* Definition,
                                 TSubclassOf<UBuzzzInstance> InstanceClass, AActor* Creator) const;
};
