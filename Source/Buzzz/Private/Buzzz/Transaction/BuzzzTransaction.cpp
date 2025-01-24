// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Transaction/BuzzzTransaction.h"
#include "Buzzz/Transaction/BuzzzTransactionBridge.h"

void UBuzzzTransaction::K2_OnExecute_Implementation()
{
}

void UBuzzzTransaction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

    // FDoRepLifetimeParams Params;
    // Params.Condition = COND_OwnerOnly;
    // Params.bIsPushBased = true;
    // DOREPLIFETIME_WITH_PARAMS(ThisClass, TransactionID, Params);
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
    State = EBuzzzExecutionState::Executing;

    GetBridge()->ReceiveTransactionStarted.Broadcast(this);

    K2_OnExecute();

    if (State == EBuzzzExecutionState::Executing)
    {
        State = EBuzzzExecutionState::Success;
    }

    GetBridge()->ReceiveTransactionFinished.Broadcast(this);

    return this;
}

void UBuzzzTransaction::K2_OnFailed_Implementation()
{
}

void UBuzzzTransaction::MarkTransactionFailed()
{
    State = EBuzzzExecutionState::Failed;

    GetBridge()->ReceiveTransactionFailed.Broadcast(this);

    K2_OnFailed();
}
