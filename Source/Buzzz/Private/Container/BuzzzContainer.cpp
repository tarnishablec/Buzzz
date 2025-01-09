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

void UBuzzzContainer::OnAssignFailed_Implementation(const FBuzzzOperationContext& Context)
{
    
}

void UBuzzzContainer::ClearCell(const int32 Index, FBuzzzOperationContext& OutContext)
{
    OutContext = FBuzzzOperationContext{};
    OutContext.TargetIndex = Index;
    OutContext.UpcomingStackCount = 0;
    AssignCell(OutContext);
}


FBuzzzOperationContext UBuzzzContainer::AssignCell_Implementation(const FBuzzzOperationContext& InContext)
{
    FScopeLock ScopeLock(&ContainerCS);

    // Make A Copy
    auto Context = InContext;
    Context.TargetContainer = this;

    // Check Index Valid
    if (!Hive.Cells.IsValidIndex(Context.TargetIndex))
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
        Context.bFinished = true;
    }

    // CheckCompatible
    if (IsValid(Context.UpcomingInstance) && !CheckItemCompatible(Context.UpcomingInstance))
    {
        Context.bFinished = true;
    }

    // Handle Failed
    if (Context.bFinished && !Context.bSuccess)
    {
        OnAssignFailed(Context);
        return Context;
    }

    // Cache PreviousCell
    FBuzzzContainerCell PreviousCellInfo;
    PreviousCellInfo.ItemInstance = Hive.Cells[Context.TargetIndex].ItemInstance;
    PreviousCellInfo.StackCount = Hive.Cells[Context.TargetIndex].StackCount;

    FBuzzzContainerCell UpcomingCellInfo;
    UpcomingCellInfo.ItemInstance = Context.UpcomingInstance;
    UpcomingCellInfo.StackCount = Context.UpcomingStackCount;

    // Fill up OutContext
    {
        Context.PreviousInstance = PreviousCellInfo.ItemInstance;
        Context.PreviousStackCount = PreviousCellInfo.StackCount;
    }

    FBuzzzCellMutationInfo MutationInfo;
    MutationInfo.Container = this;
    MutationInfo.PreviousCellInfo = PreviousCellInfo;
    MutationInfo.UpcomingCellInfo = UpcomingCellInfo;
    MutationInfo.Index = InContext.TargetIndex;

    // Pre Change Callback
    {
        PreCellChange.Broadcast(MutationInfo);
    }

    // Operate Assign
    {
        Hive.Cells[Context.TargetIndex].ItemInstance = Context.UpcomingInstance;
        Hive.Cells[Context.TargetIndex].StackCount = Context.UpcomingStackCount;
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

    // Remove Previous Replication
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

    // On Change Callback
    {
        OnCellChange.Broadcast(MutationInfo);
    }

    // FastArray Mark Dirty
    {
        Hive.MarkItemDirty(Hive.Cells[Context.TargetIndex]);
    }

    // Mark as Finished & Success
    {
        Context.bSuccess = true;
        Context.bFinished = true;
    }

    // Forward To Subsystem
    {
        const auto BuzzzSubsystem = GetOwner()->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
        if (IsValid(BuzzzSubsystem))
        {
            BuzzzSubsystem->ReceivedContainerMutation.Broadcast(Context);
        }
    }

    // Post Change Callback
    {
        PostCellChange.Broadcast(MutationInfo);
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
    Hive.Cells.SetNum(Capacity);
    Hive.MarkArrayDirty();
}

void UBuzzzContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(ThisClass, Hive, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(ThisClass, Capacity, COND_OwnerOnly);
}
