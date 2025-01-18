// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuzzzTransactionBridge.generated.h"

struct FInstancedStruct;
class UBuzzzTransaction;

UCLASS(BlueprintType, NotPlaceable)
class BUZZZ_API ABuzzzTransactionBridge : public AActor
{
    GENERATED_BODY()

public:
    ABuzzzTransactionBridge();
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
    HIDE_ACTOR_TRANSFORM_FUNCTIONS();

protected:
    UPROPERTY(ReplicatedUsing = OnRep_OwnerPlayerController)
    TObjectPtr<APlayerController> OwnerPlayerController;

    UFUNCTION()
    void OnRep_OwnerPlayerController();

public:
    UFUNCTION(Server, Reliable)
    void Server_ProcessTransaction(
        TSubclassOf<UBuzzzTransaction> TransactionClass,
        const FInstancedStruct& Payload
    );

    virtual void SetOwner(AActor* NewOwner) override;
};
