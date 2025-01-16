// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Container/BuzzzSubsystem.h"
#include "Engine/CancellableAsyncAction.h"
#include "BuzzzAction_WaitForInstanceDisconnect.generated.h"

class UBuzzzItemInstance;
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
        UBuzzzItemInstance* ItemInstance);

    virtual void Activate() override;
    virtual void Cancel() override;

    UPROPERTY(BlueprintAssignable)
    FBuzzzReceiveInstanceDisconnectDelegate Triggered;

protected:
    UPROPERTY()
    TObjectPtr<UBuzzzItemInstance> TargetItemInstance;

    UFUNCTION()
    void HandleReceivedInstanceDisconnect(UBuzzzItemInstance* ItemInstance, UBuzzzContainer* Container);
};
