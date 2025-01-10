// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "UObject/Object.h"
#include "BuzzzFastArray.generated.h"


class UBuzzzContainer;
class UBuzzzItemInstance;

USTRUCT(BlueprintType)
struct BUZZZ_API FBuzzzContainerCell : public FFastArraySerializerItem
{
    GENERATED_BODY()

    FBuzzzContainerCell()
    {
    };

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBuzzzItemInstance> ItemInstance;

    UPROPERTY(BlueprintReadOnly)
    int32 StackCount = 0;
};

USTRUCT(BlueprintType)
struct BUZZZ_API FBuzzzContainerHive : public FFastArraySerializer
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TArray<FBuzzzContainerCell> Cells;

    UPROPERTY()
    TObjectPtr<UBuzzzContainer> Container;

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
