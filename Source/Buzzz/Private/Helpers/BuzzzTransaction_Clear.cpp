// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Helpers/BuzzzTransaction_Clear.h"

#include "Container/BuzzzContainer.h"
#include "Helpers/BuzzzSharedTypes.h"

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


    FBuzzzCellOperationContext OutContext{};
    const auto Result = TargetContainer->ClearCell(TargetIndex, OutContext);

    if (!Result)
    {
        MarkTransactionFailed();
    }

}
