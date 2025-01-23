// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystem/BuzzzSubsystem.h"
#include "Engine/CancellableAsyncAction.h"
#include "BuzzzAction_WaitForInstanceDisconnect.generated.h"

class UBuzzzInstance;
class UBuzzzContainer;
/**
 * 
 */
UCLASS()
class BUZZZ_API UBuzzzAction_WaitForInstanceDisconnect : public UCancellableAsyncAction
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintInternalUseOnly, Category = "Buzzz",
        DisplayName="Wait For Disconnect From Container")
    static UBuzzzAction_WaitForInstanceDisconnect* WaitForInstanceDisconnect(
        UBuzzzInstance* ItemInstance);

    virtual void Activate() override;
    virtual void Cancel() override;

    UPROPERTY(BlueprintAssignable)
    FBuzzzInstanceDisconnectDelegate Triggered;

protected:
    UPROPERTY()
    TObjectPtr<UBuzzzInstance> TargetItemInstance;

    UFUNCTION()
    void HandleReceivedInstanceDisconnect(UBuzzzInstance* ItemInstance, const UBuzzzContainer* Container);
};
