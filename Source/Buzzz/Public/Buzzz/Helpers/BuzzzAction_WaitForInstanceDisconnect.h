// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Subsystem/BuzzzSubsystem.h"
#include "Engine/CancellableAsyncAction.h"
#include "BuzzzAction_WaitForInstanceDisconnect.generated.h"

struct FBeeepMessageListenerHandle;
class UBuzzzItem;
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
        DisplayName="Wait For Disconnect From Container", meta=(WorldContext="WorldContextObject"))
    static UBuzzzAction_WaitForInstanceDisconnect* WaitForInstanceDisconnect(
        UObject* WorldContextObject,
        UBuzzzItem* Item);

    virtual void Activate() override;
    virtual void SetReadyToDestroy() override;

    UPROPERTY(BlueprintAssignable)
    FBuzzzInstanceDisconnectDelegate Triggered;

protected:
    TWeakObjectPtr<UBuzzzItem> TargetItem;
    TWeakObjectPtr<UWorld> WorldPtr;
    FBeeepMessageListenerHandle WaitForDisconnectListenerHandle;
};
