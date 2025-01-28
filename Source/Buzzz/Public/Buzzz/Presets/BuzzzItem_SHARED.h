// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzInstancingMode_ShareDefault.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "BuzzzItem_SHARED.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, Const, BlueprintType, Abstract)
class BUZZZ_API UBuzzzItem_SHARED final : public UBuzzzItem
{
    GENERATED_BODY()

    UBuzzzItem_SHARED()
    {
        InstancingMode = UBuzzzInstancingMode_ShareDefault::StaticClass();
    };

    virtual void Kill() override
    {
        // Should Do Nothing
    };
};
