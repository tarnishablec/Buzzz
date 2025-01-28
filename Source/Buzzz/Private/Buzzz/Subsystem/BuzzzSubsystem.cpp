// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Subsystem/BuzzzSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Buzzz/Subsystem/BuzzzSettings.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Buzzz/Subsystem/BuzzzManager.h"
#include "Buzzz/Transaction/BuzzzTransaction.h"
#include "Buzzz/Transaction/BuzzzBridge.h"

void UBuzzzSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (HasAuthority())
    {
        FWorldDelegates::OnPreWorldInitialization.AddWeakLambda(
            this, [this](UWorld* World, UWorld::InitializationValues)
            {
                if (!World->IsInSeamlessTravel())
                {
                    World->OnWorldPreBeginPlay.AddWeakLambda(this, [this,World]()
                    {
                        if (IsValid(World) && World->GetNetMode() != NM_Client)
                        {
                            this->Manager = World->SpawnActor<ABuzzzManager>(UBuzzzSettings::Get()->ManagerClass);
                        }
                    });
                }
            });

        FGameModeEvents::GameModePostLoginEvent
            .AddWeakLambda(this, [this](AGameModeBase*, APlayerController* PlayerController)
            {
                if (PlayerController->HasAuthority() && !BridgeRegistry.Find(PlayerController))
                {
                    const auto NewBridget = GetWorld()->SpawnActor<ABuzzzBridge>(
                        UBuzzzSettings::Get()->TransactionBridgeClass);
                    NewBridget->SetOwner(PlayerController);
                }
            });

        FGameModeEvents::GameModeLogoutEvent.AddWeakLambda(
            this, [this](AGameModeBase*, AController* Controller)
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


void UBuzzzSubsystem::Deinitialize()
{
    Super::Deinitialize();

    FGameModeEvents::GameModePostLoginEvent.RemoveAll(this);
    FGameModeEvents::GameModeLogoutEvent.RemoveAll(this);
    // Manager->ItemRegistry.Reset();
    // Manager->Destroy();
    bInitialized = false;
}


// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void UBuzzzSubsystem::RegisterInstance(UBuzzzItem* Instance)
{
    auto& [InstanceSet] = Manager->ItemRegistry.FindOrAdd(Instance->GetClass());
    Instance->Rename(nullptr, Manager);
    InstanceSet.Add(Instance);
}

bool UBuzzzSubsystem::CheckInstanceRegistered(const UBuzzzItem* Instance) const
{
    const auto Entry = Manager->ItemRegistry.Find(Instance->GetClass());
    if (Entry)
    {
        return Entry->InstanceSet.Contains(Instance);
    }
    return false;
}

UBuzzzItem* UBuzzzSubsystem::Instantiate(
    const TSubclassOf<UBuzzzItem> InstanceClass, AActor* Creator)
{
    const auto FinalInstance = UBuzzzItem::MakeInstance_Static(InstanceClass, Creator);
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

    if (!IsValid(TransactionClass))
    {
        return;
    }

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

void UBuzzzSubsystem::RegisterBridgeLink(APlayerController* PlayerController, ABuzzzBridge* Bridge)
{
    BridgeRegistry.Add(PlayerController, Bridge);
}

void UBuzzzSubsystem::UnregisterBridgeLink(AActor* PlayerControllerOrBridge)
{
    if (const auto PC = Cast<APlayerController>(PlayerControllerOrBridge))
    {
        BridgeRegistry.Remove(PC);
    }
    else if (const auto Bridge = Cast<ABuzzzBridge>(PlayerControllerOrBridge))
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
