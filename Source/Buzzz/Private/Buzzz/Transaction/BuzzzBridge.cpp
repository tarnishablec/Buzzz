// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Transaction/BuzzzBridge.h"
#include "Buzzz/Subsystem/BuzzzSubsystem.h"
#include "Buzzz/Transaction/BuzzzBridgeBin.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Buzzz/Transaction/BuzzzTransaction.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif

// Sets default values
ABuzzzBridge::ABuzzzBridge()
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

    Bin = CreateDefaultSubobject<UBuzzzBridgeBin>(TEXT("BridgeBin"));
}

// Called when the game starts or when spawned
void ABuzzzBridge::BeginPlay()
{
    Super::BeginPlay();
}

void ABuzzzBridge::BeginDestroy()
{
    Super::BeginDestroy();
}

void ABuzzzBridge::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    UBuzzzSubsystem::Get(this)->UnregisterBridgeLink(this);
}

void ABuzzzBridge::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.Condition = COND_OwnerOnly;
    Params.bIsPushBased = true;
    DOREPLIFETIME_WITH_PARAMS(ThisClass, OwnerPlayerController, Params);
    DOREPLIFETIME_WITH_PARAMS(ThisClass, Bin, Params);
}

void ABuzzzBridge::OnRep_OwnerPlayerController()
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

UBuzzzTransaction* ABuzzzBridge::ProcessTransaction_Implementation(
    const TSubclassOf<UBuzzzTransaction> TransactionClass, const FInstancedStruct& Payload)
{
    const auto TransactionInstance = MakeTransaction(TransactionClass, Payload);
    TransactionInstance->Execute();
    return TransactionInstance;
}

UBuzzzTransaction* ABuzzzBridge::MakeTransaction_Implementation(
    const TSubclassOf<UBuzzzTransaction> TransactionClass, const FInstancedStruct& Payload)
{
    const auto Result = NewObject<UBuzzzTransaction>(this, TransactionClass);
    Result->Payload = Payload;
    return Result;
}


void ABuzzzBridge::SetOwner(AActor* NewOwner)
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
void ABuzzzBridge::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                                           UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
    UE::Net::FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif

void ABuzzzBridge::Server_ProcessTransaction_Implementation(
    const TSubclassOf<UBuzzzTransaction> TransactionClass,
    const FInstancedStruct& Payload
)
{
    ProcessTransaction(TransactionClass, Payload);
}
