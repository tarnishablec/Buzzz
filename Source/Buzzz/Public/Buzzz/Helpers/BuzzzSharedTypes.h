// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BuzzzSharedTypes.generated.h"

class UBuzzzItem;
class UBuzzzContainer;


UENUM(BlueprintType)
enum class EBuzzzExecutionState : uint8
{
    None = 0 UMETA(Hidden),
    Executing,
    Success,
    Failed,
};


UENUM(BlueprintType)
enum class EBuzzzHiveMutationType : uint8
{
    None UMETA(Hidden),
    Remove,
    Add,
    Change,
};

USTRUCT(BlueprintType)
struct BUZZZ_API FBuzzzAssignmentContext
{
    GENERATED_BODY()

    // Input

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBuzzzContainer> TargetContainer;

    UPROPERTY(BlueprintReadWrite)
    int32 TargetIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBuzzzItem> UpcomingInstance;

    UPROPERTY(BlueprintReadWrite)
    int32 UpcomingStackCount = -1;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBuzzzContainer> FromContainer;

    UPROPERTY(BlueprintReadWrite)
    int32 FromIndex = INDEX_NONE;

    // Output

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBuzzzItem> PreviousInstance;

    UPROPERTY(BlueprintReadOnly)
    int32 PreviousStackCount = -1;

    UPROPERTY(BlueprintReadOnly)
    EBuzzzExecutionState State = EBuzzzExecutionState::None;

    // UPROPERTY(BlueprintReadOnly)
    // FGuid ContextID = FGuid::NewGuid();

    static FBuzzzAssignmentContext& GetEmptyContext()
    {
        thread_local FBuzzzAssignmentContext EmptyContext{};
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

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBuzzzContainer> TargetContainer;

    UPROPERTY(BlueprintReadWrite)
    int32 TargetIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBuzzzContainer> FromContainer;

    UPROPERTY(BlueprintReadWrite)
    int32 FromIndex = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct BUZZZ_API FBuzzzHiveMutationContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBuzzzContainer> Container;

    UPROPERTY(BlueprintReadOnly)
    TArray<int32> Indices;

    UPROPERTY(BlueprintReadOnly)
    EBuzzzHiveMutationType MutationType = EBuzzzHiveMutationType::None;
};

UENUM(BlueprintType)
enum class EBuzzzItemTransferType : uint8
{
    None = 0 UMETA(Hidden),
    Addition,
    Removal
};

USTRUCT(BlueprintType)
struct BUZZZ_API FBuzzzItemTransferContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBuzzzItem> Item;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBuzzzContainer> Container;

    UPROPERTY(BlueprintReadOnly)
    EBuzzzItemTransferType TransferType = EBuzzzItemTransferType::None;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuzzzCellMutationDelegate,
                                            const FBuzzzAssignmentContext&, Context);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuzzzHiveMutationDelegate,
                                            const FBuzzzHiveMutationContext&, Context);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuzzzInstanceTransferDelegate,
                                            const FBuzzzItemTransferContext&, Context);
