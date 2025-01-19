// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Container/BuzzzSubsystem.h"

#include "GameFramework/GameModeBase.h"
#include "Transaction/BuzzzTransaction.h"
#include "Transaction/BuzzzTransactionBridge.h"

void UBuzzzSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }

    FGameModeEvents::GameModePostLoginEvent
        .AddWeakLambda(this, [this]
                   (AGameModeBase* GameMode, APlayerController* PlayerController)
                       {
                           if (PlayerController->HasAuthority() && !BridgeRegistry.Find(PlayerController))
                           {
                               const auto NewBridget = GetWorld()->SpawnActor<ABuzzzTransactionBridge>();
                               NewBridget->SetOwner(PlayerController);
                           }
                       });
}

void UBuzzzSubsystem::TryProcessTransaction(APlayerController*& Instigator,
                                            const TSubclassOf<UBuzzzTransaction>& TransactionClass,
                                            const FInstancedStruct& Payload)
{
    check(Instigator);
    check(TransactionClass->IsChildOf(UBuzzzTransaction::StaticClass()));
    
    const auto* const BridgePtr = BridgeRegistry.Find(Instigator);
    if (BridgePtr)
    {
        const auto Bridge = *BridgePtr;
        if (IsValid(Bridge))
        {
            if (Bridge->IsNetMode(NM_Client))
            {
                // Client To Server RPC
                Bridge->Server_ProcessTransaction(TransactionClass, Payload);
            }
            else
            {
                // Server Internal Call
                Bridge->ProcessTransactionByClass(TransactionClass, Payload);
            }
        }
    }
}

void UBuzzzSubsystem::RegisterBridgeLink(APlayerController* PlayerController, ABuzzzTransactionBridge* Bridge)
{
    BridgeRegistry.Add(PlayerController, Bridge);
}

void UBuzzzSubsystem::Deinitialize()
{
    Super::Deinitialize();
    FGameModeEvents::GameModePostLoginEvent.RemoveAll(this);
}
