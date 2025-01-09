// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzItemInstance.h"
#include "BuzzzItemInstance_UNIQUE.generated.h"

class UBuzzzAction_WaitForContainerOperation;
class UBuzzzContainer;
class UBuzzzItemDefinition;
struct FBuzzzOperationContext;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BUZZZ_API UBuzzzItemInstance_UNIQUE : public UBuzzzItemInstance
{
    GENERATED_BODY()

public:
    virtual UBuzzzItemInstance* MakeInstance_Implementation(const UBuzzzItemDefinition* InDefinition,
                                                            AActor* Instigator) const override;

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    UBuzzzContainer* GetOwnerContainer() const;

    virtual void BeginDestroy() override;

protected:
    UFUNCTION()
    void OnAssignAction(const FBuzzzOperationContext& Context);

    UFUNCTION()
    void OnRemoveAction(const FBuzzzOperationContext& Context);

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForContainerOperation> AssignAction;

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForContainerOperation> RemoveAction;

    virtual void ChangeOwnerContainer(UBuzzzContainer* NewContainer);

    virtual void InitializeInstance() override;
};
