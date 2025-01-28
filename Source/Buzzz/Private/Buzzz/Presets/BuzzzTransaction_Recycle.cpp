// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Buzzz/Presets/BuzzzTransaction_Recycle.h"

#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Buzzz/Presets/BuzzzTransaction_Append.h"
#include "Buzzz/Transaction/BuzzzBridge.h"


void UBuzzzTransaction_Recycle::K2_OnExecute_Implementation()
{
    const auto PayloadPtr = Payload.GetPtr<FPayloadType>();
    if (PayloadPtr == nullptr)
    {
        MarkTransactionFailed();
        return;
    }

    const auto TargetContainer = PayloadPtr->TargetContainer;
    const auto TargetIndex = PayloadPtr->TargetIndex;

    bool bIsValidIndex;
    const auto PreviousCellInfo = TargetContainer->GetCell(TargetIndex, bIsValidIndex);

    if (!bIsValidIndex || !IsValid(PreviousCellInfo.Item))
    {
        MarkTransactionFailed();
        return;
    }

    FBuzzzTransactionPayload_Append AppendPayload{};
    AppendPayload.bAutoExpand = true;
    AppendPayload.FromContainer = TargetContainer;
    AppendPayload.FromIndex = TargetIndex;
    AppendPayload.TargetContainer = GetBridge()->Bin.Get();

    const auto AppendResult = GetBridge()->ProcessTransaction<UBuzzzTransaction_Append>(
        FInstancedStruct::Make(AppendPayload));

    if (AppendResult->State != EBuzzzExecutionState::Success)
    {
        MarkTransactionFailed();
    }
}
