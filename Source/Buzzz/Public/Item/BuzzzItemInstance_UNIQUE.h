// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzItemInstance.h"
#include "BuzzzItemInstance_UNIQUE.generated.h"

class UBuzzzAction_WaitForInstanceDisconnect;
class UBuzzzAction_WaitForContainerOperation;
class UBuzzzContainer;
class UBuzzzItemDefinition;
struct FBuzzzCellAssignmentContext;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BUZZZ_API UBuzzzItemInstance_UNIQUE : public UBuzzzItemInstance
{
    GENERATED_BODY()

public:
    virtual UBuzzzItemInstance* MakeInstance_Implementation(
        const UBuzzzItemDefinition* InDefinition,
        AActor* Instigator
    ) const override;

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
    virtual void HandleDisconnectAction(UBuzzzItemInstance* ItemInstance, const UBuzzzContainer* Container);

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForContainerOperation> WaitPuInAction;

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForInstanceDisconnect> WaitDisconnectAction;

    virtual void ChangeOwnerContainer(UBuzzzContainer* NewContainer);
};
