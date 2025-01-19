// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Transaction/BuzzzTransaction.h"

#include "Net/UnrealNetwork.h"
#include "Transaction/BuzzzTransactionBridge.h"

void UBuzzzTransaction::K2_OnExecute_Implementation()
{
}

void UBuzzzTransaction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.Condition = COND_OwnerOnly;
    Params.bIsPushBased = true;
    DOREPLIFETIME_WITH_PARAMS(ThisClass, TransactionID, Params);
}

ABuzzzTransactionBridge* UBuzzzTransaction::GetBridge() const
{
    return CastChecked<ABuzzzTransactionBridge>(GetOuter());
}


APlayerController* UBuzzzTransaction::GetBridgeController() const
{
    return CastChecked<APlayerController>(GetBridge()->GetOwner());
}

const UBuzzzTransaction* UBuzzzTransaction::Execute()
{
    State = EBuzzzTransactionState::Executing;

    GetBridge()->ReceiveTransactionStarted.Broadcast(this);

    K2_OnExecute();

    if (State == EBuzzzTransactionState::Executing)
    {
        State = EBuzzzTransactionState::Success;
    }

    return this;
}

void UBuzzzTransaction::K2_OnFailed_Implementation()
{
}

void UBuzzzTransaction::MarkTransactionFailed()
{
    State = EBuzzzTransactionState::Failed;

    GetBridge()->ReceiveTransactionFinished.Broadcast(this);

    K2_OnFailed();
}
