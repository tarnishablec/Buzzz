// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzInstancingMode_PerDefinition.h"
#include "Core/Item/BuzzzInstance.h"
#include "BuzzzInstance_SHARED.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, Const, BlueprintType)
class BUZZZ_API UBuzzzInstance_SHARED final : public UBuzzzInstance
{
    GENERATED_BODY()

    UBuzzzInstance_SHARED()
    {
        InstancingMode = UBuzzzInstancingMode_PerDefinition::StaticClass();
    };
};
