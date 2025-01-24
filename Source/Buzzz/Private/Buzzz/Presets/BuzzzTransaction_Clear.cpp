// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Presets/BuzzzTransaction_Clear.h"

#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"

void UBuzzzTransaction_Clear::K2_OnExecute_Implementation()
{
    const auto PayloadPtr = Payload.GetPtr<FPayloadType>();
    if (PayloadPtr == nullptr)
    {
        MarkTransactionFailed();
        return;
    }

    const auto TargetContainer = PayloadPtr->TargetContainer;
    const auto TargetIndex = PayloadPtr->TargetIndex;


    FBuzzzCellAssignmentContext OutContext{};
    TargetContainer->ClearCell(TargetIndex, OutContext);

    if (OutContext.State != EBuzzzExecutionState::Success)
    {
        MarkTransactionFailed();
    }
}
