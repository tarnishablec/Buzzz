// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Presets/BuzzzTransaction_Swap.h"

#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"

void UBuzzzTransaction_Swap::K2_OnExecute_Implementation()
{
    const auto PayloadPtr = Payload.GetPtr<FPayloadType>();
    if (PayloadPtr == nullptr)
    {
        MarkTransactionFailed();
        return;
    }

    const auto FromContainer = PayloadPtr->FromContainer;
    const auto TargetContainer = PayloadPtr->TargetContainer;
    const auto FromIndex = PayloadPtr->FromIndex;
    const auto TargetIndex = PayloadPtr->TargetIndex;

#pragma region Validation Checks

    if (!IsValid(FromContainer) || !TargetContainer->CheckIndexIsValid(TargetIndex))
    {
        MarkTransactionFailed();
        return;
    }

    bool IsFromIndexValid;
    const auto UpcomingCellInfo = FromContainer->GetCell(FromIndex, IsFromIndexValid);

    if (!IsFromIndexValid)
    {
        MarkTransactionFailed();
        return;
    }

    // Check Is Not Same Instance So We Can Swap
    if (UpcomingCellInfo.Item == TargetContainer->GetCells()[TargetIndex].Item)
    {
        MarkTransactionFailed();
        return;
    }
#pragma endregion

    FBuzzzCellAssignmentContext InContext{};
    InContext.TargetContainer = TargetContainer;
    InContext.TargetIndex = TargetIndex;

    InContext.UpcomingStackCount = UpcomingCellInfo.StackCount;
    InContext.UpcomingInstance = UpcomingCellInfo.Item;

    InContext.FromContainer = FromContainer;
    InContext.FromIndex = FromIndex;

    TargetContainer->AssignCell(InContext);

    if (InContext.State == EBuzzzExecutionState::Success)
    {
        FBuzzzCellAssignmentContext FromContext{};
        FromContext.TargetContainer = FromContainer;
        FromContext.TargetIndex = FromIndex;

        FromContext.UpcomingStackCount = InContext.PreviousStackCount;
        FromContext.UpcomingInstance = InContext.PreviousInstance;

        FromContext.FromContainer = TargetContainer;
        FromContext.FromIndex = TargetIndex;
        FromContainer->AssignCell(FromContext);

        if (FromContext.State == EBuzzzExecutionState::Success)
        {
            return;
        }
    }

    MarkTransactionFailed();
}
