// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzInstancingMode_Unique.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "BuzzzItem_UNIQUE.generated.h"

class UBuzzzAction_WaitForItemRemovalOrAddition;
class UBuzzzAction_WaitForItemAssignment;
class UBuzzzContainer;
struct FBuzzzCellAssignmentContext;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BUZZZ_API UBuzzzItem_UNIQUE : public UBuzzzItem
{
    GENERATED_BODY()

public:
    UBuzzzItem_UNIQUE()
    {
        InstancingMode = UBuzzzInstancingMode_Unique::StaticClass();
    }

    virtual void BeginDestroy() override;

    virtual void Initialize_Implementation() override;

protected:
};
