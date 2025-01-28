// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Presets/BuzzzTransaction_Kill.h"

#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Buzzz/Presets/BuzzzTransaction_Clear.h"
#include "Buzzz/Transaction/BuzzzTransactionBridge.h"

void UBuzzzTransaction_Kill::K2_OnExecute_Implementation()
{
    const auto PayloadPtr = Payload.GetPtr<FPayloadType>();
    if (PayloadPtr == nullptr)
    {
        MarkTransactionFailed();
        return;
    }

    const auto TargetContainer = PayloadPtr->TargetContainer;
    const auto TargetIndex = PayloadPtr->TargetIndex;


    FBuzzzAssignmentContext OutContext{};
    TargetContainer->ClearCell(TargetIndex, OutContext);

    if (OutContext.State != EBuzzzExecutionState::Success)
    {
        MarkTransactionFailed();
        return;
    }

    const auto Item = OutContext.PreviousInstance;
    Item.Get()->Demolish();
}
