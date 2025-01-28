// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "BuzzzTransaction.generated.h"

class UBuzzzSubsystem;
class ABuzzzBridge;

/**
 * 
 */
UCLASS(Blueprintable, Abstract, meta=(ShowWorldContextPin))
class BUZZZ_API UBuzzzTransaction : public UObject
{
    GENERATED_BODY()

public:
    UBuzzzTransaction()
    {
        TransactionID = FGuid::NewGuid();
    }

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadOnly)
    FGuid TransactionID;

    UPROPERTY(BlueprintReadOnly, Category="Buzzz")
    FInstancedStruct Payload;

    UPROPERTY(BlueprintReadOnly, Category="Buzzz")
    EBuzzzExecutionState State = EBuzzzExecutionState::None;

    UFUNCTION(BlueprintPure, Category="Buzzz")
    ABuzzzBridge* GetBridge() const;

    UFUNCTION(BlueprintPure, Category="Buzzz")
    APlayerController* GetBridgeController() const;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz")
    virtual const UBuzzzTransaction* Execute();

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category="Buzzz", DisplayName="OnExecute")
    void K2_OnExecute();

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category="Buzzz", DisplayName="OnFailed")
    void K2_OnFailed();

protected:
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Buzzz",
        meta=(BlueprintProtected, hideSelfPin=true))
    void MarkTransactionFailed();
};
