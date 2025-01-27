// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Subsystem/BuzzzSubsystem.h"
#include "Engine/CancellableAsyncAction.h"
#include "BuzzzAction_WaitForItemRemovalOrAddition.generated.h"

struct FBeeepMessageListenerHandle;
class UBuzzzItem;
class UBuzzzContainer;
/**
 * 
 */
UCLASS()
class BUZZZ_API UBuzzzAction_WaitForItemRemovalOrAddition : public UCancellableAsyncAction
{
    GENERATED_BODY()

    enum class EAdditionOrRemoval : uint8
    {
        None = 0,
        Addition,
        Removal
    };

public:
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintInternalUseOnly, Category = "Buzzz",
        DisplayName="Wait For Removal From Container", meta=(WorldContext="WorldContextObject"))
    static UBuzzzAction_WaitForItemRemovalOrAddition* WaitForRemoval(
        UObject* WorldContextObject,
        UBuzzzItem* Item);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintInternalUseOnly, Category = "Buzzz",
        DisplayName="Wait For Addition To Container", meta=(WorldContext="WorldContextObject"))
    static UBuzzzAction_WaitForItemRemovalOrAddition* WaitForAddition(
        UObject* WorldContextObject,
        UBuzzzItem* Item);

    virtual void Activate() override;
    virtual void SetReadyToDestroy() override;

    UPROPERTY(BlueprintAssignable)
    FBuzzzInstanceDisconnectDelegate Triggered;

protected:
    EAdditionOrRemoval ActionMode = EAdditionOrRemoval::None;
    TWeakObjectPtr<UBuzzzItem> TargetItem;
    TWeakObjectPtr<UWorld> WorldPtr;
    FBeeepMessageListenerHandle ListenerHandle;
};
