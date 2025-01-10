// Copyright 2019-Present tarnishablec. All Rights Reserved.


// ReSharper disable CppMemberFunctionMayBeConst
#include "Container/BuzzzFastArray.h"

#include "Container/BuzzzContainer.h"

void FBuzzzContainerHive::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
    if (IsValid(Container) && RemovedIndices.Num() > 0)
    {
        Container->Client_ReceiveHiveMutation.Broadcast(TArray(RemovedIndices.GetData(), RemovedIndices.Num()),
                                                EBuzzzHiveMutationType::Remove);
    }
}

void FBuzzzContainerHive::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
    if (IsValid(Container) && AddedIndices.Num() > 0)
    {
        Container->Client_ReceiveHiveMutation.Broadcast(TArray(AddedIndices.GetData(), AddedIndices.Num()),
                                                EBuzzzHiveMutationType::Add);
    }
}

void FBuzzzContainerHive::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
    if (IsValid(Container) && ChangedIndices.Num() > 0)
    {
        Container->Client_ReceiveHiveMutation.Broadcast(TArray(ChangedIndices.GetData(), ChangedIndices.Num()),
                                                EBuzzzHiveMutationType::Change);
    }
}
