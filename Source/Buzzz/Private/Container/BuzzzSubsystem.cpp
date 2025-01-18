// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Container/BuzzzSubsystem.h"

#include "GameFramework/GameModeBase.h"
#include "Transaction/BuzzzTransaction.h"
#include "Transaction/BuzzzTransactionBridge.h"

void UBuzzzSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
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

void UBuzzzSubsystem::ExecuteTransaction(APlayerController* Instigator,
                                         const TSubclassOf<UBuzzzTransaction>& TransactionClass,
                                         const FInstancedStruct& Payload)
{
    check(Instigator);
    const auto* Bridge = BridgeRegistry.Find(Instigator);
    if (Bridge && IsValid(*Bridge))
    {
        (*Bridge)->Server_ProcessTransaction(TransactionClass, Payload);
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
