// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Transaction/BuzzzTransactionBridge.h"
#include "Buzzz/Subsystem/BuzzzSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Buzzz/Transaction/BuzzzTransaction.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif

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

void ABuzzzTransactionBridge::BeginDestroy()
{
    Super::BeginDestroy();
}

void ABuzzzTransactionBridge::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    UBuzzzSubsystem::Get(this)->UnregisterBridgeLink(this);
}

void ABuzzzTransactionBridge::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.Condition = COND_OwnerOnly;
    Params.bIsPushBased = true;
    DOREPLIFETIME_WITH_PARAMS(ThisClass, OwnerPlayerController, Params);
}

void ABuzzzTransactionBridge::OnRep_OwnerPlayerController()
{
    if (OwnerPlayerController)
    {
        UBuzzzSubsystem::Get(this)->RegisterBridgeLink(OwnerPlayerController, this);
    }
    else
    {
        checkNoEntry();
    }
}

UBuzzzTransaction* ABuzzzTransactionBridge::ProcessTransaction_Implementation(
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


    if (HasAuthority() && GetWorld()->IsGameWorld())
    {
        COMPARE_ASSIGN_AND_MARK_PROPERTY_DIRTY(ThisClass, OwnerPlayerController, Cast<APlayerController>(NewOwner),
                                               this);

        if (OwnerPlayerController)
        {
            UBuzzzSubsystem::Get(this)->RegisterBridgeLink(OwnerPlayerController, this);
        }
        else
        {
            UBuzzzSubsystem::Get(this)->UnregisterBridgeLink(this);
        }
    }
}

#if UE_WITH_IRIS
void ABuzzzTransactionBridge::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                                           UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
    UE::Net::FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif

void ABuzzzTransactionBridge::Server_ProcessTransaction_Implementation(
    const TSubclassOf<UBuzzzTransaction> TransactionClass,
    const FInstancedStruct& Payload
)
{
    ProcessTransaction(TransactionClass, Payload);
}
