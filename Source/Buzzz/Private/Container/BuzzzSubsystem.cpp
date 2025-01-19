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

    FGameModeEvents::GameModeLogoutEvent.AddWeakLambda(this, [this](AGameModeBase* GameMode, AController* Controller)
    {
        const auto PC = CastChecked<APlayerController>(Controller);
        if (PC->HasAuthority())
        {
            const auto MatchedBridgePtr = BridgeRegistry.Find(PC);
            if (MatchedBridgePtr && IsValid(*MatchedBridgePtr))
            {
                (*MatchedBridgePtr)->SetOwner(nullptr);
                (*MatchedBridgePtr)->Destroy();
            }
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

void UBuzzzSubsystem::UnregisterBridgeLink(AActor* PlayerControllerOrBridge)
{
    if (const auto PC = Cast<APlayerController>(PlayerControllerOrBridge))
    {
        BridgeRegistry.Remove(PC);
    }
    else if (const auto Bridge = Cast<ABuzzzTransactionBridge>(PlayerControllerOrBridge))
    {
        const auto PCPtr = BridgeRegistry.FindKey(Bridge);
        if (PCPtr)
        {
            BridgeRegistry.Remove(*PCPtr);
        }
    }
}

void UBuzzzSubsystem::Deinitialize()
{
    Super::Deinitialize();
    FGameModeEvents::GameModePostLoginEvent.RemoveAll(this);
    FGameModeEvents::GameModeLogoutEvent.RemoveAll(this);
}
