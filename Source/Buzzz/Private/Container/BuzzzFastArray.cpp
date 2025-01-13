// Copyright 2019-Present tarnishablec. All Rights Reserved.


// ReSharper disable CppMemberFunctionMayBeConst

#include "Container/BuzzzFastArray.h"

#include "Algo/ForEach.h"
#include "Container/BuzzzContainer.h"
#include "Item/BuzzzItemInstance.h"

void FBuzzzContainerHive::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
    if (IsValid(Container) && RemovedIndices.Num() > 0)
    {
        Container->Client_ReceiveHiveMutation.Broadcast(
            Container, TArray(RemovedIndices.GetData(), RemovedIndices.Num()),
            EBuzzzHiveMutationType::Remove);
    }
}

void FBuzzzContainerHive::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, const int32 FinalSize)
{
    /**
     * Since Fast Array replication batches Add and Change operations,
     * We need to separate them for specific client-side UI updates.
     *
     * This also ensures consistency with the behavior in standalone mode.
     */
    if (IsValid(Container) && AddedIndices.Num() > 0)
    {
        TArray<int32> ChangedIndices{};

        Algo::ForEach(AddedIndices, [this, &ChangedIndices](const int32& Index)
        {
            if (IsValid(Cells[Index].ItemInstance))
            {
                ChangedIndices.Add(Index);
            }
        });

        Container->Client_ReceiveHiveMutation.Broadcast(Container, TArray(AddedIndices.GetData(), AddedIndices.Num()),
                                                        EBuzzzHiveMutationType::Add);

        if (ChangedIndices.Num() > 0)
        {
            PostReplicatedChange(ChangedIndices, FinalSize);
        }
    }
}

void FBuzzzContainerHive::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
    if (IsValid(Container) && ChangedIndices.Num() > 0)
    {
        Container->Client_ReceiveHiveMutation.Broadcast(
            Container, TArray(ChangedIndices.GetData(), ChangedIndices.Num()),
            EBuzzzHiveMutationType::Change);
    }
}
