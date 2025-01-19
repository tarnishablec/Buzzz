// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Transaction/BuzzzTransactionBridge.h"

#include "Container/BuzzzSubsystem.h"
// #include "Net/UnrealNetwork.h"
#include "Transaction/BuzzzTransaction.h"

// Sets default values
ABuzzzTransactionBridge::ABuzzzTransactionBridge()
{
    PrimaryActorTick.bCanEverTick = false;
    bNetLoadOnClient = false;
    bOnlyRelevantToOwner = true;
    bNetUseOwnerRelevancy = true;
    bReplicates = true;
    SetHidden(true);
    SetReplicatingMovement(false);
    SetCanBeDamaged(false);
    bEnableAutoLODGeneration = false;
}

// Called when the game starts or when spawned
void ABuzzzTransactionBridge::BeginPlay()
{
    Super::BeginPlay();
}

void ABuzzzTransactionBridge::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABuzzzTransactionBridge::OnRep_OwnerPlayerController()
{
    // Client SetOwner
    SetOwner(OwnerPlayerController);
}

UBuzzzTransaction* ABuzzzTransactionBridge::ProcessTransactionByClass_Implementation(
    const TSubclassOf<UBuzzzTransaction> TransactionClass, const FInstancedStruct& Payload)
{
    const auto TransactionInstance = MakeTransaction(TransactionClass, Payload);
    TransactionInstance->Execute();
    return TransactionInstance;
}

UBuzzzTransaction* ABuzzzTransactionBridge::MakeTransaction_Implementation(
    const TSubclassOf<UBuzzzTransaction> TransactionClass, const FInstancedStruct& Payload)
{
    const auto Result = NewObject<UBuzzzTransaction>(this, TransactionClass);
    Result->Payload = Payload;
    return Result;
}


void ABuzzzTransactionBridge::SetOwner(AActor* NewOwner)
{
    Super::SetOwner(NewOwner);
    OwnerPlayerController = Cast<APlayerController>(NewOwner);
    if (OwnerPlayerController)
    {
        const auto Subsystem = GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
        Subsystem->RegisterBridgeLink(OwnerPlayerController, this);
    }
}

void ABuzzzTransactionBridge::Server_ProcessTransaction_Implementation(
    const TSubclassOf<UBuzzzTransaction> TransactionClass,
    const FInstancedStruct& Payload
)
{
    ProcessTransactionByClass(TransactionClass, Payload);
}
