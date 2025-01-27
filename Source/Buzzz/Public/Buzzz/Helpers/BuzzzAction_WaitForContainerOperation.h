// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Engine/CancellableAsyncAction.h"
#include "BuzzzAction_WaitForContainerOperation.generated.h"

class UBuzzzSubsystem;
struct FBuzzzCellAssignmentContext;
class UBuzzzItem;
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
        DisplayName="Wait For Assigned To Cell", meta=(WorldContext="WorldContextObject"))
    static UBuzzzAction_WaitForContainerOperation* WaitForAssignToCell(UWorld* WorldContextObject,
                                                                       UBuzzzItem* Item);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", BlueprintInternalUseOnly,
        DisplayName="Wait For Cleared From Cell", meta=(WorldContext="WorldContextObject"))
    static UBuzzzAction_WaitForContainerOperation* WaitForClearedFromCell(UWorld* WorldContextObject,
                                                                          UBuzzzItem* Item);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", BlueprintInternalUseOnly,
        DisplayName="Wait For Put In Container", meta=(WorldContext="WorldContextObject"))
    static UBuzzzAction_WaitForContainerOperation* WaitForPutInContainer(UWorld* WorldContextObject,
                                                                         UBuzzzItem* Item);

protected:
    TWeakObjectPtr<UBuzzzItem> TargetItem;
    TWeakObjectPtr<UWorld> WorldPtr;
    EOperationMode OperationMode;
    TSharedPtr<FBeeepMessageListenerHandle> Handle = MakeShared<FBeeepMessageListenerHandle>();
};
