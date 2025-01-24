// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Subsystem/BuzzzSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Helpers/BuzzzSettings.h"
#include "Core/Item/BuzzzDefinition.h"
#include "Core/Item/BuzzzInstance.h"
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
                    if (MatchedBridgePtr && IsValid(MatchedBridgePtr->Get()))
                    {
                        MatchedBridgePtr->Get()->SetOwner(nullptr);
                        MatchedBridgePtr->Get()->Destroy();
                    }
                }
            });
    }

    bInitialized = true;
}

void UBuzzzSubsystem::RegisterInstance(UBuzzzInstance* Instance)
{
    if (const auto Definition = Instance->GetDefinition())
    {
        check(IsValid(Definition))
        auto [InstanceSet] = ItemRegistry.FindOrAdd(Definition->DefinitionGuid);
        InstanceSet.Add(Instance);
    }
    else
    {
        auto [InstanceSet] = ItemRegistry.FindOrAdd(FGuid());
        InstanceSet.Add(Instance);
    }
}

bool UBuzzzSubsystem::CheckInstanceRegistered(const UBuzzzInstance* Instance) const
{
    if (const auto Definition = Instance->GetDefinition())
    {
        if (const auto* Entry = ItemRegistry.Find(Definition->DefinitionGuid))
        {
            return Entry->InstanceSet.Contains(Instance);
        }
        return false;
    }
    if (const auto* Entry = ItemRegistry.Find(FGuid()))
    {
        return Entry->InstanceSet.Contains(Instance);
    }
    return false;
}

TSet<UBuzzzInstance*> UBuzzzSubsystem::GetInstancesByDefinition(
    const UBuzzzDefinition* Definition) const
{
    if (IsValid(Definition))
    {
        return ItemRegistry.Find(Definition->DefinitionGuid)->InstanceSet;
    }
    return ItemRegistry.Find(FGuid())->InstanceSet;
}

UBuzzzInstance* UBuzzzSubsystem::Instantiate(const UBuzzzDefinition* Definition,
                                             const TSubclassOf<UBuzzzInstance> InstanceClass, AActor* Creator)
{
    const auto FinalInstance = UBuzzzInstancingMode::MakeInstance_Static(Definition, InstanceClass, Creator);
    if (IsValid(FinalInstance))
    {
        this->RegisterInstance(FinalInstance);
        FinalInstance->Initialize();
    }
    return FinalInstance;
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

    const auto Bridge = BridgeRegistry.Find(Instigator);
    if (Bridge->IsValid())
    {
        if (IsValid(Bridge->Get()))
        {
            Bridge->Get()->Server_ProcessTransaction(TransactionClass, Payload);
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
    return IsTemplate() ? ETickableTickType::Never : ETickableTickType::Conditional;
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
    ItemRegistry.Reset();
    bInitialized = false;
}
