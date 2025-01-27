// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "UObject/Object.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "BuzzzFastArray.generated.h"


class UBuzzzContainer;
class UBuzzzItem;


USTRUCT(BlueprintType)
struct BUZZZ_API FBuzzzContainerCell : public FFastArraySerializerItem
{
    GENERATED_BODY()

    FBuzzzContainerCell()
    {
    };

    UPROPERTY(BlueprintReadOnly, Transient)
    TObjectPtr<UBuzzzItem> Item;

    UPROPERTY(BlueprintReadOnly, Transient)
    int32 StackCount = 0;

    UPROPERTY(Transient)
    TObjectPtr<UBuzzzContainer> Container;

    UPROPERTY(Transient)
    int32 Index = INDEX_NONE;
};


USTRUCT(BlueprintType)
struct BUZZZ_API FBuzzzContainerHive : public FFastArraySerializer
{
    GENERATED_BODY()

    DECLARE_MULTICAST_DELEGATE_TwoParams(FInternalHiveMutationDelegate,
                                         const TArray<int32>&,
                                         const EBuzzzHiveMutationType);

    FInternalHiveMutationDelegate ReceiveRemoteHiveMutation;

    UPROPERTY(BlueprintReadOnly)
    TArray<FBuzzzContainerCell> Cells;

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
    {
        return FastArrayDeltaSerialize<FBuzzzContainerCell, FBuzzzContainerHive>(
            Cells, DeltaParams, *this);
    }

    void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);

    void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);

    void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
};


template <>
struct TStructOpsTypeTraits<FBuzzzContainerHive> : TStructOpsTypeTraitsBase2<FBuzzzContainerHive>
{
    enum { WithNetDeltaSerializer = true };
};
