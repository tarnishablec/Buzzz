// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Container/BuzzzContainer.h"

#include "Container/BuzzzSubsystem.h"
#include "Item/BuzzzItemInstance.h"
#include "Net/UnrealNetwork.h"

UBuzzzContainer::UBuzzzContainer()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
    bReplicateUsingRegisteredSubObjectList = true;
    bWantsInitializeComponent = true;
}

bool UBuzzzContainer::CheckItemInstanceOwned(const UBuzzzItemInstance* ItemInstance) const
{
    for (auto&& Cell : Hive.Cells)
    {
        if (Cell.ItemInstance == ItemInstance)
        {
            return true;
        }
    }

    return false;
}

bool UBuzzzContainer::CheckCellEmpty(const int32& Index) const
{
    if (!Hive.Cells.IsValidIndex(Index))
    {
        return true;
    }

    return Hive.Cells[Index].ItemInstance != nullptr;
}

const FBuzzzContainerCell& UBuzzzContainer::GetCell(const int32& Index, bool& IsValidIndex) const
{
    static const FBuzzzContainerCell EmptyCell;
    if (!Hive.Cells.IsValidIndex(Index))
    {
        IsValidIndex = false;
        return EmptyCell;
    }
    IsValidIndex = true;
    return Hive.Cells[Index];
}

bool UBuzzzContainer::CheckCellHasItem(const int32& Index, const UBuzzzItemDefinition* ItemDefinition) const
{
    const auto Instance = Hive.Cells[Index].ItemInstance;
    if (Instance == nullptr)
    {
        return false;
    }

    return Instance->GetDefinition() == ItemDefinition;
}

int32 UBuzzzContainer::CalcTotalAmountOfItem(const UBuzzzItemDefinition* ItemDefinition)
{
    int32 Result = 0;
    for (int Index = 0; Index < Hive.Cells.Num(); ++Index)
    {
        if (CheckCellHasItem(Index, ItemDefinition))
        {
            Result += Hive.Cells[Index].StackCount;
        }
    }

    return Result;
}

int32 UBuzzzContainer::FindEmptySlot(bool& Found) const
{
    int32 Index = INDEX_NONE;
    Found = false;
    for (int i = 0; i < Hive.Cells.Num(); ++i)
    {
        if (CheckCellEmpty(i))
        {
            Index = i;
            Found = true;
            break;
        }
    }
    return Index;
}

bool UBuzzzContainer::CheckIndexIsValid(const int32& Index) const
{
    return Hive.Cells.IsValidIndex(Index);
}

bool UBuzzzContainer::Resize(const int32& NewCapacity)
{
    check(NewCapacity >= 0);

    for (int i = NewCapacity; i < Capacity; ++i)
    {
        FBuzzzOperationContext Context{};
        ClearCell(i, Context);

        if (!(Context.bFinished && Context.bSuccess))
        {
            return false;
        }
    }

    Capacity = NewCapacity;
    Hive.Cells.SetNum(Capacity);
    Hive.MarkArrayDirty();

    return true;
}


bool UBuzzzContainer::ClearCell_Implementation(const int32& Index, FBuzzzOperationContext& OutContext)
{
    if (!CheckIndexIsValid(Index))
    {
        return false;
    }

    OutContext.Reset();
    OutContext.TargetIndex = Index;
    OutContext.UpcomingStackCount = 0;
    OutContext.TargetContainer = this;
    AssignCell(OutContext);

    return OutContext.bFinished && OutContext.bSuccess;
}

bool UBuzzzContainer::MergeCells_Implementation(const int32& Index, UBuzzzContainer* FromContainer,
                                                const int32& FromIndex)
{
#pragma region Validation Checks
    if (!IsValid(FromContainer) || !CheckIndexIsValid(Index))
    {
        return false;
    }

    bool IsFromIndexValid;
    const auto UpcomingCellInfo = FromContainer->GetCell(FromIndex, IsFromIndexValid);

    if (!IsFromIndexValid)
    {
        return false;
    }

    // Check Is Same Instance So We Can Merge
    if (UpcomingCellInfo.ItemInstance != Hive.Cells[Index].ItemInstance)
    {
        return false;
    }
#pragma endregion

    FBuzzzOperationContext InContext{};
    InContext.TargetContainer = this;
    InContext.TargetIndex = Index;

    InContext.UpcomingStackCount = UpcomingCellInfo.StackCount + Hive.Cells[Index].StackCount;
    InContext.UpcomingInstance = UpcomingCellInfo.ItemInstance;

    InContext.FromContainer = FromContainer;
    InContext.FromIndex = FromIndex;
    AssignCell(InContext);

    if (InContext.bFinished && InContext.bSuccess)
    {
        FBuzzzOperationContext FromContext{};
        const auto Result = FromContainer->ClearCell(FromIndex, FromContext);
        check(Result);

        return Result;
    }

    return false;
}


bool UBuzzzContainer::SwapCells_Implementation(const int32& Index, UBuzzzContainer* FromContainer,
                                               const int32& FromIndex)
{
#pragma region Validation Checks

    if (!IsValid(FromContainer) || !CheckIndexIsValid(Index))
    {
        return false;
    }

    bool IsFromIndexValid;
    const auto UpcomingCellInfo = FromContainer->GetCell(FromIndex, IsFromIndexValid);

    if (!IsFromIndexValid)
    {
        return false;
    }

    // Check Is Not Same Instance So We Can Swap
    if (UpcomingCellInfo.ItemInstance == Hive.Cells[Index].ItemInstance)
    {
        return false;
    }
#pragma endregion

    FBuzzzOperationContext InContext{};
    InContext.TargetContainer = this;
    InContext.TargetIndex = Index;

    InContext.UpcomingStackCount = UpcomingCellInfo.StackCount;
    InContext.UpcomingInstance = UpcomingCellInfo.ItemInstance;

    InContext.FromContainer = FromContainer;
    InContext.FromIndex = FromIndex;

    AssignCell(InContext);

    if (InContext.bFinished && InContext.bSuccess)
    {
        FBuzzzOperationContext FromContext{};
        FromContext.TargetContainer = FromContainer;
        FromContext.TargetIndex = FromIndex;

        FromContext.UpcomingStackCount = InContext.PreviousStackCount;
        FromContext.UpcomingInstance = InContext.PreviousInstance;

        FromContext.FromContainer = this;
        FromContext.FromIndex = Index;
        FromContainer->AssignCell(FromContext);

        return FromContext.bFinished && FromContext.bSuccess;
    }

    return false;
}


FBuzzzOperationContext UBuzzzContainer::AssignCell_Implementation(FBuzzzOperationContext& Context)
{
    FScopeLock ScopeLock(&ContainerCS);

    // Make Sure TargetContainer is Set
    Context.TargetContainer = this;

    // Check Index Valid
    if (!CheckIndexIsValid(Context.TargetIndex))
    {
        Context.bFinished = true;
    }
    // Nothing Changed
    else if (Hive.Cells[Context.TargetIndex].ItemInstance == Context.UpcomingInstance && Hive.Cells[Context.TargetIndex]
        .StackCount == Context.UpcomingStackCount)
    {
        Context.bFinished = true;
    }

    // StackCount Should be Positive
    check(Context.UpcomingStackCount >= 0);
    if (Context.UpcomingStackCount < 0)
    {
        Context.bFinished = true;
    }

    // if Empty Instance, StackCount Should Be 0
    if (!IsValid(Context.UpcomingInstance))
    {
        check(Context.UpcomingStackCount == 0);
        if (Context.UpcomingStackCount != 0)
        {
            Context.bFinished = true;
        }
    }

    // CheckCompatible
    if (IsValid(Context.UpcomingInstance) && !CheckItemCompatible(Context.UpcomingInstance))
    {
        Context.bFinished = true;
    }

    // Handle Failed
    if (Context.bFinished && !Context.bSuccess)
    {
        OnAssignFailed.Broadcast(Context);
        return Context;
    }


    // Fill up OutContext, Cache Previous Info
    {
        Context.PreviousInstance = Hive.Cells[Context.TargetIndex].ItemInstance;
        Context.PreviousStackCount = Hive.Cells[Context.TargetIndex].StackCount;
    }

    // Pre Change Callback
    {
        PreCellChange.Broadcast(Context);
    }

    // Operate Assign
    {
        Hive.Cells[Context.TargetIndex].ItemInstance = Context.UpcomingInstance;
        Hive.Cells[Context.TargetIndex].StackCount = Context.UpcomingStackCount;
    }

    // Remove Previous Replication First
    if (IsValid(Context.PreviousInstance))
    {
        if (Context.PreviousInstance->ShouldReplicate)
        {
            RemoveReplicatedSubObject(Context.PreviousInstance);
            TArray<UObject*> NetObjList{};
            Context.PreviousInstance->GetSubobjectsWithStableNamesForNetworking(NetObjList);
            for (auto&& NetSubObject : NetObjList)
            {
                RemoveReplicatedSubObject(NetSubObject);
            }
        }
    }

    // Set Upcoming Replication
    if (IsValid(Context.UpcomingInstance))
    {
        if (Context.UpcomingInstance->ShouldReplicate)
        {
            AddReplicatedSubObject(Context.UpcomingInstance);
            TArray<UObject*> NetObjList{};
            Context.UpcomingInstance->GetSubobjectsWithStableNamesForNetworking(NetObjList);
            for (auto&& NetSubObject : NetObjList)
            {
                AddReplicatedSubObject(NetSubObject);
            }
        }
    }

    // On Change Callback
    {
        OnCellChange.Broadcast(Context);
    }

    // FastArray Mark Dirty
    {
        Hive.MarkItemDirty(Hive.Cells[Context.TargetIndex]);
    }

    // Mark as Success
    {
        Context.bSuccess = true;
    }

    // Mark as Finished
    {
        Context.bFinished = true;
    }

    // Post Change Callback
    {
        PostCellChange.Broadcast(Context);
    }

    // Forward To Subsystem
    {
        const auto BuzzzSubsystem = GetOwner()->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
        if (IsValid(BuzzzSubsystem))
        {
            BuzzzSubsystem->ReceivedContainerMutation.Broadcast(Context);
        }
    }

    return Context;
}

void UBuzzzContainer::InitializeComponent()
{
    Super::InitializeComponent();
}


bool UBuzzzContainer::CheckItemCompatible_Implementation(const UBuzzzItemInstance* ItemInstance) const
{
    return true;
}


void UBuzzzContainer::BeginPlay()
{
    Super::BeginPlay();
    Resize(Capacity);
}

void UBuzzzContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(ThisClass, Hive, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(ThisClass, Capacity, COND_OwnerOnly);
}
