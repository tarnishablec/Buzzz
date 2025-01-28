// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Presets/BuzzzTransaction_Append.h"

#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Buzzz/Presets/BuzzzTransaction_Merge.h"
#include "Buzzz/Presets/BuzzzTransaction_Swap.h"
#include "Buzzz/Transaction/BuzzzBridge.h"

void UBuzzzTransaction_Append::K2_OnExecute_Implementation()
{
    const auto InPayload = Payload.GetPtr<FPayloadType>();
    if (InPayload == nullptr)
    {
        MarkTransactionFailed();
        return;
    }

    const auto FromContainer = InPayload->FromContainer;
    const auto TargetContainer = InPayload->TargetContainer;
    const auto FromIndex = InPayload->FromIndex;


#pragma region Validation Checks
    bool IsFromIndexValid;
    const auto UpcomingCellInfo = FromContainer->GetCell(FromIndex, IsFromIndexValid);

    if (!IsFromIndexValid || !IsValid(UpcomingCellInfo.Item))
    {
        MarkTransactionFailed();
        return;
    }

    if (!IsValid(TargetContainer))
    {
        MarkTransactionFailed();
        return;
    }
#pragma endregion

    TArray<int32> OutIndices;
    int32 OutFirst;
    int32 OutLast;
    bool bFound;
    TargetContainer->FindIndexByInstance(UpcomingCellInfo.Item, OutIndices, OutFirst,
                                         OutLast, bFound);

    FBuzzzTransactionPayload_Common NextPayload;
    NextPayload.FromContainer = InPayload->FromContainer;
    NextPayload.FromIndex = InPayload->FromIndex;
    NextPayload.TargetContainer = InPayload->TargetContainer;

    if (bFound)
    {
        NextPayload.TargetIndex = OutFirst;
        const auto MergeResult = GetBridge()->ProcessTransaction<UBuzzzTransaction_Merge>(
            FInstancedStruct::Make(NextPayload));
        if (MergeResult->State != EBuzzzExecutionState::Success)
        {
            MarkTransactionFailed();
            return;
        }
    }
    else
    {
        bool bFoundEmpty;
        const auto EmptyIndex = TargetContainer->FindEmptyCell(bFoundEmpty);

        if (!bFoundEmpty)
        {
            if (InPayload->bAutoExpand)
            {
                NextPayload.TargetIndex = TargetContainer->GetCapacity();
                const auto ResizeResult = TargetContainer->Resize((TargetContainer->GetCapacity() + 1) * 1.5);
                if (!ResizeResult)
                {
                    MarkTransactionFailed();
                    return;
                }
            }
            else
            {
                MarkTransactionFailed();
                return;
            }
        }
        else
        {
            NextPayload.TargetIndex = EmptyIndex;
        }

        check(
            TargetContainer->CheckIndexIsValid(NextPayload.TargetIndex)
            && TargetContainer->CheckCellEmpty(NextPayload.TargetIndex)
        );

        const auto SwapResult = GetBridge()->ProcessTransaction<UBuzzzTransaction_Swap>(
            FInstancedStruct::Make(NextPayload));
        if (SwapResult->State != EBuzzzExecutionState::Success)
        {
            MarkTransactionFailed();
        }

        return;
    }
    MarkTransactionFailed();
}
