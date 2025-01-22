// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BuzzzSharedTypes.generated.h"

class UBuzzzItemInstance;
class UBuzzzContainer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBuzzzInstanceDisconnectDelegate, UBuzzzItemInstance*,
                                             ItemInstance, const UBuzzzContainer*, Container);

UENUM(BlueprintType)
enum class EBuzzzExecutionState : uint8
{
    None = 0 UMETA(Hidden),
    Executing,
    Success,
    Failed,
};

USTRUCT(BlueprintType)
struct BUZZZ_API FBuzzzCellAssignmentContext
{
    GENERATED_BODY()

    // Input

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBuzzzContainer> TargetContainer;

    UPROPERTY(BlueprintReadWrite)
    int32 TargetIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBuzzzItemInstance> UpcomingInstance;

    UPROPERTY(BlueprintReadWrite)
    int32 UpcomingStackCount = -1;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBuzzzContainer> FromContainer;

    UPROPERTY(BlueprintReadWrite)
    int32 FromIndex = INDEX_NONE;

    // Output

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBuzzzItemInstance> PreviousInstance;

    UPROPERTY(BlueprintReadOnly)
    int32 PreviousStackCount = -1;

    UPROPERTY(BlueprintReadOnly)
    EBuzzzExecutionState State = EBuzzzExecutionState::None;

    static FBuzzzCellAssignmentContext& GetEmptyContext()
    {
        thread_local FBuzzzCellAssignmentContext EmptyContext{};
        return EmptyContext;
    }

    void Reset()
    {
        *this = GetEmptyContext();
    }
};

USTRUCT(BlueprintType)
struct FBuzzzTransactionPayload_Common
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category="Buzzz")
    TObjectPtr<UBuzzzContainer> TargetContainer;

    UPROPERTY(BlueprintReadWrite, Category="Buzzz")
    int32 TargetIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadWrite, Category="Buzzz")
    TObjectPtr<UBuzzzContainer> FromContainer;

    UPROPERTY(BlueprintReadWrite, Category="Buzzz")
    int32 FromIndex = INDEX_NONE;
};
