// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Container/BuzzzContainer.h"
#include "Engine/CancellableAsyncAction.h"
#include "BuzzzAction_WaitForContainerOperation.generated.h"

class UBuzzzSubsystem;
struct FBuzzzCellAssignmentContext;
class UBuzzzInstance;
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
        Clear,
        PutIn,
    };

    UPROPERTY(BlueprintAssignable)
    FBuzzzCellMutationDelegate Triggered;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", BlueprintInternalUseOnly,
        DisplayName="Wait For Assigned To Cell")
    static UBuzzzAction_WaitForContainerOperation* WaitForAssignToCell(UBuzzzInstance* ItemInstance);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", BlueprintInternalUseOnly,
        DisplayName="Wait For Cleared From Cell")
    static UBuzzzAction_WaitForContainerOperation* WaitForClearedFromCell(
        UBuzzzInstance* ItemInstance);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", BlueprintInternalUseOnly,
        DisplayName="Wait For Put In Container")
    static UBuzzzAction_WaitForContainerOperation* WaitForPutInContainer(
        UBuzzzInstance* ItemInstance);

protected:
    UPROPERTY()
    TObjectPtr<UBuzzzInstance> TargetItemInstance;

    EOperationMode OperationMode;

    UFUNCTION()
    void HandleReceivedContainerMutation(const FBuzzzCellAssignmentContext& Context);
};
