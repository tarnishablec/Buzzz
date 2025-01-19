// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuzzzContainer.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Transaction/BuzzzTransactionBridge.h"
#include "BuzzzSubsystem.generated.h"

struct FInstancedStruct;
class ABuzzzTransactionBridge;
class UBuzzzTransaction;
class UBuzzzItemInstance;
struct FBuzzzCellOperationContext;
class UBuzzzContainer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBuzzzReceiveInstanceDisconnectDelegate, UBuzzzItemInstance*,
                                             ItemInstance, UBuzzzContainer*, Container);

/**
 * 
 */
UCLASS()
class BUZZZ_API UBuzzzSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzCellMutationDelegate ReceiveContainerCellMutation;

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzReceiveInstanceDisconnectDelegate ReceiveInstanceDisconnect;


    UFUNCTION(BlueprintCallable, Category="Buzzz", meta = (AutoCreateRefTerm="TransactionClass,Payload"))
    void TryProcessTransaction(UPARAM(ref) APlayerController*& Instigator,
                               const TSubclassOf<UBuzzzTransaction>& TransactionClass,
                               const FInstancedStruct& Payload);

    // This Will Run Both in Server And Client
    UFUNCTION()
    void RegisterBridgeLink(APlayerController* PlayerController, ABuzzzTransactionBridge* Bridge);

protected:
    friend class UBuzzzTransactionBridge;

    UPROPERTY()
    TMap<APlayerController*, ABuzzzTransactionBridge*> BridgeRegistry;
};
