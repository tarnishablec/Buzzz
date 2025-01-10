// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Container/BuzzzContainer.h"
#include "Engine/CancellableAsyncAction.h"
#include "BuzzzAction_WaitForContainerOperation.generated.h"

class UBuzzzSubsystem;
struct FBuzzzCellOperationContext;
class UBuzzzItemInstance;
/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuzzzTask_WaitForContainerOperationTriggered,
                                            const FBuzzzCellOperationContext&, Context);

UCLASS()
class BUZZZ_API UBuzzzAction_WaitForContainerOperation : public UCancellableAsyncAction
{
    GENERATED_BODY()
public:
    virtual void Activate() override;
    virtual void Cancel() override;

    enum EOperationMode
    {
        Assign,
        Remove
    };

    UPROPERTY(BlueprintAssignable)
    FBuzzzTask_WaitForContainerOperationTriggered Triggered;

    UFUNCTION(BlueprintCallable, Category = "Buzzz", BlueprintInternalUseOnly, DisplayName="Wait For Assigned To Cell")
    static UBuzzzAction_WaitForContainerOperation* WaitForAssignToCell(UBuzzzItemInstance* ItemInstance);

    UFUNCTION(BlueprintCallable, Category = "Buzzz", BlueprintInternalUseOnly, DisplayName="Wait For Removed From Cell")
    static UBuzzzAction_WaitForContainerOperation* WaitForRemoveFromCell(
        UBuzzzItemInstance* ItemInstance);

protected:
    UPROPERTY()
    TObjectPtr<UBuzzzItemInstance> ItemInstance;

    EOperationMode OperationMode;

    UFUNCTION()
    void HandleReceivedContainerMutation(const FBuzzzCellOperationContext& Context);
};
