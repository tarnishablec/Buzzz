// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Container/BuzzzContainer.h"
#include "Engine/CancellableAsyncAction.h"
#include "BuzzzAction_WaitForContainerOperation.generated.h"

class UBuzzzSubsystem;
struct FBuzzzOperationContext;
class UBuzzzItemInstance;
/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuzzzTask_WaitForContainerOperationTriggered,
                                            const FBuzzzOperationContext&, Context);

UCLASS()
class BUZZZ_API UBuzzzAction_WaitForContainerOperation : public UCancellableAsyncAction
{
    GENERATED_BODY()

    virtual void Activate() override;

    enum EOperationMode
    {
        Assign,
        Remove
    };

public:
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
    void HandleReceivedContainerMutation(const FBuzzzOperationContext& Context);

public:
    virtual void BeginDestroy() override
    {
        Super::BeginDestroy();
    }

    virtual void Cancel() override;
};
