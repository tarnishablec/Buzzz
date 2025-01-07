// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzItemInstance.h"
#include "BuzzzItemInstance_SHARED.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, Const, BlueprintType)
class BUZZZ_API UBuzzzItemInstance_SHARED : public UBuzzzItemInstance
{
    GENERATED_BODY()

    UBuzzzItemInstance_SHARED();

public:
    virtual UBuzzzItemInstance* MakeInstance_Implementation(const UBuzzzItemDefinition* InDefinition) const override;

protected:
    static TMap<const UBuzzzItemDefinition*, UBuzzzItemInstance_SHARED*> InstanceMap;
};
