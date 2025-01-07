// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzItemDefinition.h"
#include "BuzzzItemInstance.h"
#include "BuzzzItemInstance_UNIQUE.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BUZZZ_API UBuzzzItemInstance_UNIQUE : public UBuzzzItemInstance
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "Buzzz")
    void OnInitialized();

    virtual UBuzzzItemInstance* MakeInstance_Implementation(const UBuzzzItemDefinition* InDefinition) const override;
};
