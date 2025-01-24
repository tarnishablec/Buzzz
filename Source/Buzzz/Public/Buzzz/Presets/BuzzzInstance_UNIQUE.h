// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzInstancingMode_PerItem.h"
#include "Buzzz/Core/Item/BuzzzInstance.h"
#include "BuzzzInstance_UNIQUE.generated.h"

class UBuzzzAction_WaitForInstanceDisconnect;
class UBuzzzAction_WaitForContainerOperation;
class UBuzzzContainer;
class UBuzzzDefinition;
struct FBuzzzCellAssignmentContext;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BUZZZ_API UBuzzzInstance_UNIQUE : public UBuzzzInstance
{
    GENERATED_BODY()

public:
    UBuzzzInstance_UNIQUE()
    {
        InstancingMode = UBuzzzInstancingMode_PerItem::StaticClass();
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
    virtual void HandleDisconnectAction(UBuzzzInstance* ItemInstance, const UBuzzzContainer* Container);

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForContainerOperation> WaitPuInAction;

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForInstanceDisconnect> WaitDisconnectAction;

    virtual void ChangeOwnerContainer(UBuzzzContainer* NewContainer);
};
