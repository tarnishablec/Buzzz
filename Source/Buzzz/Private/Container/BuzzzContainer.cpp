// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Container/BuzzzContainer.h"
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

void UBuzzzContainer::AssignCell_Implementation(FBuzzzContainerAssignOperationContext& Context,
                                                FBuzzzContainerAssignOperationContext& OutContext)
{
    FScopeLock ScopeLock(&ContainerCS);

    Context.TargetContainer = this;
    if (!IsValid(Context.TargetContainer) || !Context.TargetContainer->Hive.Cells.IsValidIndex(Context.TargetIndex))
    {
        Context.bFinished = true;
        return;
    }

    if (!CheckItemCompatible(Context.ItemInstance))
    {
        Context.bFinished = true;
        return;
    }

    // Cache Previous Instance
    {
        Context.PreviousCellInfo.ItemInstance = Hive.Cells[Context.TargetIndex].ItemInstance;
        Context.PreviousCellInfo.StackCount = Hive.Cells[Context.TargetIndex].StackCount;
    }

    // Nothing Changed
    if (Context.PreviousCellInfo.ItemInstance == Context.ItemInstance
        && Context.PreviousCellInfo.StackCount == Context.FinalStackCount)
    {
        Context.bFinished = true;
        return;
    }

    // Operate Assign
    {
        Hive.Cells[Context.TargetIndex].ItemInstance = Context.ItemInstance;
        if (Context.ItemInstance == nullptr)
        {
            ensure(Context.FinalStackCount == 0);
        }
        Hive.Cells[Context.TargetIndex].StackCount = Context.FinalStackCount;
    }

    // Set Outer | Replicate
    if (IsValid(Context.ItemInstance))
    {
        Context.ItemInstance->Rename(nullptr, this);

        if (Context.ItemInstance->ShouldReplicate)
        {
            TArray<UObject*> NetObjList{};
            Context.ItemInstance->GetSubobjectsWithStableNamesForNetworking(NetObjList);

            if (IsValid(Context.SourceContainer))
            {
                Context.SourceContainer->RemoveReplicatedSubObject(Context.ItemInstance);
                for (auto&& NetSubObject : NetObjList)
                {
                    Context.SourceContainer->RemoveReplicatedSubObject(NetSubObject);
                }
            }
            AddReplicatedSubObject(Context.ItemInstance);
            for (auto&& NetSubObject : NetObjList)
            {
                AddReplicatedSubObject(NetSubObject);
            }
        }
    }

    Hive.MarkItemDirty(Hive.Cells[Context.TargetIndex]);
    Context.bSuccess = true;
    Context.bFinished = true;
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
