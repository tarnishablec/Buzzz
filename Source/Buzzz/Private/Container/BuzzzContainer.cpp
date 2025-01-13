// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Container/BuzzzContainer.h"

#include "Container/BuzzzSubsystem.h"
#include "Item/BuzzzItemInstance.h"
#include "Net/UnrealNetwork.h"

// void UBuzzzContainer::OnRep_Hive_Implementation()
// {
// }

UBuzzzContainer::UBuzzzContainer()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
    bReplicateUsingRegisteredSubObjectList = true;
    bWantsInitializeComponent = true;
    Hive.Container = this;
}

int32 UBuzzzContainer::GetCapacity() const
{
    return Hive.Cells.Num();
}

const TArray<FBuzzzContainerCell>& UBuzzzContainer::GetCells() const
{
    return Hive.Cells;
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

bool UBuzzzContainer::CheckCellHasItemByDefinition(const int32& Index, const UBuzzzItemDefinition* ItemDefinition) const
{
    const auto Instance = Hive.Cells[Index].ItemInstance;
    if (Instance == nullptr)
    {
        return false;
    }

    return Instance->GetDefinition() == ItemDefinition;
}

bool UBuzzzContainer::CheckCellHasItemByDefinitionClass(const int32& Index,
                                                        const TSubclassOf<UBuzzzItemDefinition>& DefinitionClass,
                                                        const bool bStrict) const
{
    const auto Instance = Hive.Cells[Index].ItemInstance;
    if (Instance == nullptr)
    {
        return false;
    }

    if (bStrict)
    {
        return Instance->GetDefinition()->GetClass() == DefinitionClass;
    }

    return Instance->GetDefinition()->IsA(DefinitionClass);
}

int32 UBuzzzContainer::CalcTotalAmountOfItem(const UBuzzzItemDefinition* ItemDefinition) const
{
    int32 Result = 0;
    for (int Index = 0; Index < Hive.Cells.Num(); ++Index)
    {
        if (CheckCellHasItemByDefinition(Index, ItemDefinition))
        {
            Result += Hive.Cells[Index].StackCount;
        }
    }

    return Result;
}

int32 UBuzzzContainer::FindEmptyCell(bool& Found) const
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

void UBuzzzContainer::FindIndexByInstance(const UBuzzzItemInstance* ItemInstance, TArray<int32>& OutIndexArray,
                                          int32& First,
                                          int32& Last, bool& Found) const
{
    First = INDEX_NONE;
    Last = INDEX_NONE;
    OutIndexArray = {};

    for (int Index = 0; Index < Hive.Cells.Num(); ++Index)
    {
        if (Hive.Cells[Index].ItemInstance == ItemInstance)
        {
            OutIndexArray.Add(Index);
        }
    }

    Found = OutIndexArray.Num() > 0;
    if (Found)
    {
        First = OutIndexArray[0];
        Last = OutIndexArray.Last();
    }
}

void UBuzzzContainer::FindIndexByDefinition(const UBuzzzItemDefinition* Definition, bool bStrict,
                                            TArray<int32>& OutIndexArray,
                                            int32& First, int32& Last, bool& Found) const
{
    First = INDEX_NONE;
    Last = INDEX_NONE;
    OutIndexArray = {};

    for (int Index = 0; Index < Hive.Cells.Num(); ++Index)
    {
        if (CheckCellHasItemByDefinition(Index, Definition))
        {
            OutIndexArray.Add(Index);
        }
    }

    Found = OutIndexArray.Num() > 0;
    if (Found)
    {
        First = OutIndexArray[0];
        Last = OutIndexArray.Last();
    }
}

void UBuzzzContainer::Standalone_TrySubmitMutations()
{
    check(GetNetMode()==NM_Standalone);

    if (Standalone_Batched_RemovedIndices.Num())
    {
        Client_ReceiveHiveMutation.Broadcast(this, Standalone_Batched_RemovedIndices, EBuzzzHiveMutationType::Remove);
    }

    if (Standalone_Batched_AddedIndices.Num())
    {
        Client_ReceiveHiveMutation.Broadcast(this, Standalone_Batched_AddedIndices, EBuzzzHiveMutationType::Add);
    }

    if (Standalone_Batched_ChangedIndices.Num())
    {
        Client_ReceiveHiveMutation.Broadcast(this, Standalone_Batched_ChangedIndices, EBuzzzHiveMutationType::Change);
    }


    Standalone_Batched_RemovedIndices.Reset();
    Standalone_Batched_AddedIndices.Reset();
    Standalone_Batched_ChangedIndices.Reset();
}

void UBuzzzContainer::Standalone_HandlePostCellChanged(const FBuzzzCellOperationContext& Context)
{
    check(GetNetMode()==NM_Standalone);

    Standalone_Batched_ChangedIndices.AddUnique(Context.TargetIndex);
}

void UBuzzzContainer::Standalone_HandleHiveResize(const UBuzzzContainer* Container, const TArray<int32>& Indices,
                                                  const EBuzzzHiveMutationType ResizeType)
{
    check(GetNetMode()==NM_Standalone);

    if (ResizeType == EBuzzzHiveMutationType::Add)
    {
        for (auto Index : Indices)
        {
            Standalone_Batched_AddedIndices.AddUnique(Index);
        }
    }

    if (ResizeType == EBuzzzHiveMutationType::Remove)
    {
        for (auto Index : Indices)
        {
            Standalone_Batched_ChangedIndices.AddUnique(Index);
        }
    }
}

bool UBuzzzContainer::Resize(const int32& NewCapacity)
{
    check(NewCapacity >= 0);

    if (NewCapacity == GetCapacity())
    {
        return false;
    }

    TArray<int32> RemovedIndices{};
    TArray<int32> AddedIndices{};

    // Clear Cell While NewCapacity is smaller
    for (int i = NewCapacity; i < GetCapacity(); ++i)
    {
        FBuzzzCellOperationContext Context{};
        ClearCell(i, Context);

        if (!(Context.bFinished && Context.bSuccess))
        {
            return false;
        }
        RemovedIndices.Add(i);
    }

    for (int i = GetCapacity(); i < NewCapacity; ++i)
    {
        AddedIndices.Add(i);
    }

    Hive.Cells.SetNum(NewCapacity);
    Hive.MarkArrayDirty();


    if (RemovedIndices.Num() > 0)
    {
        OnHiveResize.Broadcast(this, RemovedIndices, EBuzzzHiveMutationType::Remove);
    }

    if (AddedIndices.Num() > 0)
    {
        OnHiveResize.Broadcast(this, AddedIndices, EBuzzzHiveMutationType::Add);
    }

    return true;
}


bool UBuzzzContainer::ClearCell_Implementation(const int32& Index, FBuzzzCellOperationContext& OutContext)
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

    FBuzzzCellOperationContext InContext{};
    InContext.TargetContainer = this;
    InContext.TargetIndex = Index;

    InContext.UpcomingStackCount = UpcomingCellInfo.StackCount + Hive.Cells[Index].StackCount;
    InContext.UpcomingInstance = UpcomingCellInfo.ItemInstance;

    InContext.FromContainer = FromContainer;
    InContext.FromIndex = FromIndex;
    AssignCell(InContext);

    if (InContext.bFinished && InContext.bSuccess)
    {
        FBuzzzCellOperationContext FromContext{};
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

    FBuzzzCellOperationContext InContext{};
    InContext.TargetContainer = this;
    InContext.TargetIndex = Index;

    InContext.UpcomingStackCount = UpcomingCellInfo.StackCount;
    InContext.UpcomingInstance = UpcomingCellInfo.ItemInstance;

    InContext.FromContainer = FromContainer;
    InContext.FromIndex = FromIndex;

    AssignCell(InContext);

    if (InContext.bFinished && InContext.bSuccess)
    {
        FBuzzzCellOperationContext FromContext{};
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


FBuzzzCellOperationContext UBuzzzContainer::AssignCell_Implementation(FBuzzzCellOperationContext& Context)
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

    // Fill up OutContext, Cache Previous Info
    {
        Context.PreviousInstance = Hive.Cells[Context.TargetIndex].ItemInstance;
        Context.PreviousStackCount = Hive.Cells[Context.TargetIndex].StackCount;
    }

    // Handle Failed
    if (Context.bFinished && !Context.bSuccess)
    {
        OnAssignFailed.Broadcast(Context);
        return Context;
    }

    // Server Pre Change Callback
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

    // Server On Change Callback
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

    // Forward To Subsystem
    {
        const auto BuzzzSubsystem = GetOwner()->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
        if (IsValid(BuzzzSubsystem))
        {
            BuzzzSubsystem->ReceivedContainerMutation.Broadcast(Context);
        }
    }

    // Server Post Change Callback
    {
        PostCellChange.Broadcast(Context);
    }

    return Context;
}

// void UBuzzzContainer::OnRep_Hive_Implementation()
// {
// }

void UBuzzzContainer::InitializeComponent()
{
    Super::InitializeComponent();

    // Standalone 
    if (GetNetMode() == NM_Standalone)
    {
        PostCellChange.AddDynamic(this, &UBuzzzContainer::Standalone_HandlePostCellChanged);
        OnHiveResize.AddDynamic(this, &UBuzzzContainer::Standalone_HandleHiveResize);
    }

    OnInitialization();

    if (GetOwner()->HasAuthority())
    {
        Resize(InitialCapacity);
    }
}

void UBuzzzContainer::TickComponent(const float DeltaTime, const enum ELevelTick TickType,
                                    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (GetNetMode() == NM_Standalone)
    {
        Standalone_TrySubmitMutations();
    }
}


bool UBuzzzContainer::CheckItemCompatible_Implementation(const UBuzzzItemInstance* ItemInstance) const
{
    return true;
}


void UBuzzzContainer::BeginPlay()
{
    Super::BeginPlay();
}

void UBuzzzContainer::BeginDestroy()
{
    Super::BeginDestroy();
}

void UBuzzzContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(ThisClass, Hive, COND_OwnerOnly);
}

void UBuzzzContainer::OnInitialization_Implementation()
{
}
