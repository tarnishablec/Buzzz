// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Presets/BuzzzTransaction_Merge.h"

#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "Buzzz/Presets/BuzzzTransaction_Clear.h"
#include "Buzzz/Transaction/BuzzzTransactionBridge.h"

void UBuzzzTransaction_Merge::K2_OnExecute_Implementation()
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

    // Check Is Same Instance So We Can Merge
    if (UpcomingCellInfo.Item != TargetContainer->GetCells()[TargetIndex].Item)
    {
        MarkTransactionFailed();
        return;
    }
#pragma endregion

    FBuzzzCellAssignmentContext InContext{};
    InContext.TargetContainer = TargetContainer;
    InContext.TargetIndex = TargetIndex;

    InContext.UpcomingStackCount = UpcomingCellInfo.StackCount + TargetContainer->GetCells()[TargetIndex].StackCount;
    InContext.UpcomingInstance = UpcomingCellInfo.Item;

    InContext.FromContainer = FromContainer;
    InContext.FromIndex = FromIndex;
    TargetContainer->AssignCell(InContext);

    if (InContext.State != EBuzzzExecutionState::Success)
    {
        MarkTransactionFailed();
        return;
    }

    FBuzzzTransactionPayload_Common ClearPayload{};
    ClearPayload.TargetContainer = FromContainer;
    ClearPayload.TargetIndex = FromIndex;
    const auto InstancedPayload = FInstancedStruct::Make(ClearPayload);
    const auto ClearTransactionInstance = GetBridge()->ProcessTransaction<UBuzzzTransaction_Clear>(
        InstancedPayload);

    if (ClearTransactionInstance->State == EBuzzzExecutionState::Success)
    {
        return;
    }

    MarkTransactionFailed();
}
