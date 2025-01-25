// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzInstancingMode_Unique.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "BuzzzItem_UNIQUE.generated.h"

class UBuzzzAction_WaitForInstanceDisconnect;
class UBuzzzAction_WaitForContainerOperation;
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

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Buzzz")
    AActor* GetOwnerActor() const;

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    UBuzzzContainer* GetOwnerContainer() const;

    virtual void BeginDestroy() override;

    virtual void Initialize_Implementation() override;

protected:
    UFUNCTION()
    virtual void HandlePutInAction(const FBuzzzCellAssignmentContext& Context);

    UFUNCTION()
    virtual void HandleDisconnectAction(UBuzzzItem* Item, const UBuzzzContainer* Container);

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForContainerOperation> WaitPuInAction;

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForInstanceDisconnect> WaitDisconnectAction;

    virtual void ChangeOwnerContainer(UBuzzzContainer* NewContainer);
};
