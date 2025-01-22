// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuzzzContainer.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Helpers/BuzzzSharedTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Transaction/BuzzzTransactionBridge.h"
#include "BuzzzSubsystem.generated.h"

struct FInstancedStruct;
class ABuzzzTransactionBridge;
class UBuzzzTransaction;
class UBuzzzItemInstance;
struct FBuzzzCellAssignmentContext;
class UBuzzzContainer;


/**
 * 
 */
UCLASS()
class BUZZZ_API UBuzzzSubsystem final : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzCellMutationDelegate ReceiveContainerCellMutation;

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzInstanceDisconnectDelegate ReceiveInstanceDisconnect;


    UFUNCTION(BlueprintCallable, Category="Buzzz", BlueprintCosmetic,
        meta = (AutoCreateRefTerm="TransactionClass,Payload"))
    void Try_Process_Server_Transaction(
        UPARAM(ref) APlayerController*& Instigator,
        UPARAM(meta=(AllowAbstract=false)) const TSubclassOf<UBuzzzTransaction>& TransactionClass,
        const FInstancedStruct& Payload
    );

    // This Will Run Both in Server And Client
    void RegisterBridgeLink(APlayerController* PlayerController, ABuzzzTransactionBridge* Bridge);

    void UnregisterBridgeLink(AActor* PlayerControllerOrBridge);
    virtual void Tick(float DeltaTime) override;

    virtual TStatId GetStatId() const override
    {
        return Super::GetStatID();
    };
    virtual UWorld* GetTickableGameObjectWorld() const override;
    virtual ETickableTickType GetTickableTickType() const override;
    virtual bool IsTickable() const override { return !IsTemplate() && bInitialized; };
    virtual bool IsTickableWhenPaused() const override { return true; };

    UFUNCTION(BlueprintPure, Category="Buzzz")
    bool HasAuthority() const;

protected:
    friend class UBuzzzTransactionBridge;

    bool bInitialized = false;

    UPROPERTY()
    TMap<APlayerController*, ABuzzzTransactionBridge*> BridgeRegistry;
};
