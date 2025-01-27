// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Engine/CancellableAsyncAction.h"
#include "BuzzzAction_WaitForItemAssignment.generated.h"

class UBuzzzSubsystem;
struct FBuzzzAssignmentContext;
class UBuzzzItem;
/**
 * 
 */

UCLASS()
class BUZZZ_API UBuzzzAction_WaitForItemAssignment : public UCancellableAsyncAction
{
    GENERATED_BODY()

public:
    virtual void Activate() override;
    virtual void Cancel() override;

    UPROPERTY(BlueprintAssignable)
    FBuzzzCellMutationDelegate Triggered;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", BlueprintInternalUseOnly,
        DisplayName="Wait For Assigned To Cell", meta=(WorldContext="WorldContextObject"))
    static UBuzzzAction_WaitForItemAssignment* WaitForAssignToCell(
        const UWorld* WorldContextObject, UBuzzzItem* Item);

protected:
    TWeakObjectPtr<UBuzzzItem> TargetItem;
    TWeakObjectPtr<UWorld> WorldPtr;
    TSharedPtr<FBeeepMessageListenerHandle> Handle = MakeShared<FBeeepMessageListenerHandle>();
};
