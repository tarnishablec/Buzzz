// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzInstancingMode_Unique.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "BuzzzItem_UNIQUE.generated.h"

class UBuzzzAction_WaitForItemRemovalOrAddition;
class UBuzzzAction_WaitForItemAssignment;
class UBuzzzContainer;
struct FBuzzzAssignmentContext;

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
    virtual void Initialize() override;

    virtual void PreDemolish_Implementation() override;

protected:
    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForItemRemovalOrAddition> AdditionAction;

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForItemRemovalOrAddition> RemovalAction;

    UFUNCTION()
    void HandleAdditionToContainer(const FBuzzzItemTransferContext& Context);

    UFUNCTION()
    void HandleRemovalFromContainer(const FBuzzzItemTransferContext& Context);
};
