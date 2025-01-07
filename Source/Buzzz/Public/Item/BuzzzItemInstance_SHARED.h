// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzItemInstance.h"
#include "BuzzzItemInstance_SHARED.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, Const)
class BUZZZ_API UBuzzzItemInstance_SHARED : public UBuzzzItemInstance
{
    GENERATED_BODY()

    UBuzzzItemInstance_SHARED();

    virtual void InitializeInstance(const UBuzzzItemDefinition* ItemDefinition) override;

protected:
    UPROPERTY()
    TMap<const UBuzzzItemDefinition*, UBuzzzItemInstance_SHARED*> InstanceMap = {};
};
