// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Container/BuzzzSubsystem.h"

#include "GameFramework/GameModeBase.h"
#include "Helpers/BuzzzSettings.h"
#include "Transaction/BuzzzTransaction.h"
#include "Transaction/BuzzzTransactionBridge.h"

void UBuzzzSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (HasAuthority())
    {
        FGameModeEvents::GameModePostLoginEvent
            .AddWeakLambda(this, [this]
                       (AGameModeBase* GameMode, APlayerController* PlayerController)
                           {
                               if (PlayerController->HasAuthority() && !BridgeRegistry.Find(PlayerController))
                               {
                                   const auto NewBridget = GetWorld()->SpawnActor<ABuzzzTransactionBridge>(
                                       UBuzzzSettings::Get()->TransactionBridgeClass);
                                   NewBridget->SetOwner(PlayerController);
                               }
                           });

        FGameModeEvents::GameModeLogoutEvent.AddWeakLambda(
            this, [this](AGameModeBase* GameMode, AController* Controller)
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

    bInitialized = true;
}

void UBuzzzSubsystem::Try_Process_Server_Transaction(APlayerController*& Instigator,
                                                     const TSubclassOf<UBuzzzTransaction>& TransactionClass,
                                                     const FInstancedStruct& Payload)
{
    check(Instigator);
    check(TransactionClass->IsChildOf(UBuzzzTransaction::StaticClass()));

    if (Instigator->IsNetMode(NM_DedicatedServer))
    {
        checkNoEntry();
    }

    const auto* const BridgePtr = BridgeRegistry.Find(Instigator);
    if (BridgePtr)
    {
        const auto Bridge = *BridgePtr;
        if (IsValid(Bridge))
        {
            Bridge->Server_ProcessTransaction(TransactionClass, Payload);
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

void UBuzzzSubsystem::Tick(float DeltaTime)
{
}


UWorld* UBuzzzSubsystem::GetTickableGameObjectWorld() const
{
    return GetGameInstance()->GetWorld();
}

ETickableTickType UBuzzzSubsystem::GetTickableTickType() const
{
    return IsTemplate() ? ETickableTickType::Never : FTickableGameObject::GetTickableTickType();
}

bool UBuzzzSubsystem::HasAuthority() const
{
    const auto NetMode = GetWorld()->GetNetMode();
    return NetMode == NM_Standalone || NetMode == NM_DedicatedServer || NetMode == NM_ListenServer;
}

void UBuzzzSubsystem::Deinitialize()
{
    Super::Deinitialize();
    FGameModeEvents::GameModePostLoginEvent.RemoveAll(this);
    FGameModeEvents::GameModeLogoutEvent.RemoveAll(this);
    bInitialized = false;
}
