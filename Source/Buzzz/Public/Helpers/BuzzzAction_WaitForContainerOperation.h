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

UCLASS()
class BUZZZ_API UBuzzzAction_WaitForContainerOperation : public UCancellableAsyncAction
{
    GENERATED_BODY()

public:
   
    virtual void Activate() override;
    virtual void Cancel() override;

    enum class EOperationMode:uint8
    {
        Assign,
        Remove,
        PutIn,
        Disconnect
    };

    UPROPERTY(BlueprintAssignable)
    FBuzzzCellMutationDelegate Triggered;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", BlueprintInternalUseOnly,
        DisplayName="Wait For Assigned To Cell")
    static UBuzzzAction_WaitForContainerOperation* WaitForAssignToCell(UBuzzzItemInstance* ItemInstance);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", BlueprintInternalUseOnly,
        DisplayName="Wait For Removed From Cell")
    static UBuzzzAction_WaitForContainerOperation* WaitForRemoveFromCell(
        UBuzzzItemInstance* ItemInstance);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", BlueprintInternalUseOnly,
        DisplayName="Wait For Put In Container")
    static UBuzzzAction_WaitForContainerOperation* WaitForPutInContainer(
        UBuzzzItemInstance* ItemInstance, UBuzzzContainer* Container);

protected:
    UPROPERTY()
    TObjectPtr<UBuzzzItemInstance> TargetItemInstance;

    UPROPERTY()
    TObjectPtr<UBuzzzContainer> TargetContainer;

    EOperationMode OperationMode;

    UFUNCTION()
    void HandleReceivedContainerMutation(const FBuzzzCellOperationContext& Context);

};
