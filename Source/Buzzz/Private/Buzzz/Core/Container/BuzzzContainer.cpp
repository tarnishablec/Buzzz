// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Net/UnrealNetwork.h"
#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Subsystem/BuzzzSubsystem.h"

UE_DEFINE_GAMEPLAY_TAG(Tag_BuzzzEvent, "BuzzzEvent");
UE_DEFINE_GAMEPLAY_TAG(Tag_BuzzzEvent_CellMutation, "BuzzzEvent.CellMutation");
UE_DEFINE_GAMEPLAY_TAG(Tag_BuzzzEvent_HiveResize, "BuzzzEvent.HiveResize");

UBuzzzContainer::UBuzzzContainer()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
    bReplicateUsingRegisteredSubObjectList = true;
    bWantsInitializeComponent = true;

    ContainerGuid = FGuid::NewGuid();

    Hive.OwningObject = this;

#if UE_WITH_IRIS
#endif
}

#pragma region Helpers

bool UBuzzzContainer::CheckItemCompatible_Implementation(const UBuzzzItem* Item) const
{
    return true;
}

int32 UBuzzzContainer::GetCapacity() const
{
    return Hive.Cells.Num();
}

const TArray<FBuzzzContainerCell>& UBuzzzContainer::GetCells() const
{
    return Hive.Cells;
}

bool UBuzzzContainer::CheckItemOwned(const UBuzzzItem* Item) const
{
    for (auto&& Cell : Hive.Cells)
    {
        if (Cell.Item == Item)
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

    return Hive.Cells[Index].Item != nullptr;
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

int32 UBuzzzContainer::CalcTotalAmountByInstance(const UBuzzzItem* Item) const
{
    int32 Result = 0;
    for (int Index = 0; Index < Hive.Cells.Num(); ++Index)
    {
        if (Item == Hive.Cells[Index].Item)
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

void UBuzzzContainer::FindIndexByInstance(const UBuzzzItem* Item, TArray<int32>& OutIndexArray,
                                          int32& First,
                                          int32& Last, bool& Found) const
{
    First = INDEX_NONE;
    Last = INDEX_NONE;
    OutIndexArray = {};

    for (int Index = 0; Index < Hive.Cells.Num(); ++Index)
    {
        if (Hive.Cells[Index].Item == Item)
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

void UBuzzzContainer::FindIndexByInstanceClass(const TSubclassOf<UBuzzzItem>& InstanceClass,
                                               const bool Explicit,
                                               TArray<int32>& OutIndexArray,
                                               int32& First, int32& Last, bool& Found) const
{
    check(IsValid(InstanceClass))

    First = INDEX_NONE;
    Last = INDEX_NONE;
    OutIndexArray = {};

    for (int Index = 0; Index < Hive.Cells.Num(); ++Index)
    {
        if (Explicit
                ? (Hive.Cells[Index].Item.GetClass() == InstanceClass)
                : (Hive.Cells[Index].Item.IsA(InstanceClass)))
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

int32 UBuzzzContainer::CalcTotalAmount(UBuzzzItem* Instance)
{
    int32 Result = 0;
    for (int Index = 0; Index < Hive.Cells.Num(); ++Index)
    {
        if (Instance == Hive.Cells[Index].Item)
        {
            Result += Hive.Cells[Index].StackCount;
        }
    }

    return Result;
}

void UBuzzzContainer::MarkIndexDirty(const int32 Index, const bool bForce)
{
    if (bForce)
    {
        Hive.Cells[Index].ForceDirtyFlag = !Hive.Cells[Index].ForceDirtyFlag;
    }
    Hive.MarkItemDirty(Hive.Cells[Index]);
}
#pragma endregion

void UBuzzzContainer::Internal_Locally_TrySubmitMutationInfoToClient()
{
    check(GetNetMode()==NM_Standalone || GetNetMode() == NM_ListenServer);

    if (Internal_Batched_RemovedIndices.Num())
    {
        Client_ReceiveHiveMutation.Broadcast({this, Internal_Batched_RemovedIndices, EBuzzzHiveMutationType::Remove});
    }

    if (Internal_Batched_AddedIndices.Num())
    {
        Client_ReceiveHiveMutation.Broadcast({this, Internal_Batched_AddedIndices, EBuzzzHiveMutationType::Add});
    }

    if (Internal_Batched_ChangedIndices.Num())
    {
        Client_ReceiveHiveMutation.Broadcast({this, Internal_Batched_ChangedIndices, EBuzzzHiveMutationType::Change});
    }
}

void UBuzzzContainer::Internal_HandlePostCellChanged(const FBuzzzAssignmentContext& Context)
{
    // if (GetNetMode() == NM_Standalone)
    {
        Internal_Batched_ChangedIndices.AddUnique(Context.TargetIndex);
    }

    {
        if (IsValid(Context.PreviousInstance))
        {
            Internal_MayBeDisconnected_Instances.AddUnique(Context.PreviousInstance);
        }
    }
}

void UBuzzzContainer::Internal_HandlePostHiveResize(const FBuzzzHiveMutationContext& Context)
{
    // TODO : Should be Optimized
    {
        if (Context.MutationType == EBuzzzHiveMutationType::Add)
        {
            for (auto Index : Context.Indices)
            {
                const auto Count = Internal_Batched_RemovedIndices.Remove(Index);
                if (Count == 0)
                {
                    Internal_Batched_AddedIndices.AddUnique(Index);
                }
            }
        }

        if (Context.MutationType == EBuzzzHiveMutationType::Remove)
        {
            for (auto Index : Context.Indices)
            {
                const auto Count = Internal_Batched_AddedIndices.Remove(Index);
                if (Count == 0)
                {
                    Internal_Batched_RemovedIndices.AddUnique(Index);
                }
            }
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
        FBuzzzAssignmentContext Context{};
        ClearCell(i, Context);

        if (Context.State != EBuzzzExecutionState::Success)
        {
            return false;
        }
        RemovedIndices.Add(i);
    }

    const auto OldCapacity = GetCapacity();

    Hive.Cells.SetNum(NewCapacity);

    for (int i = OldCapacity; i < NewCapacity; ++i)
    {
        AddedIndices.Add(i);
        Hive.Cells[i].Container = this;
        Hive.Cells[i].Index = i;
    }

    Hive.MarkArrayDirty();


    if (RemovedIndices.Num() > 0)
    {
        const FBuzzzHiveMutationContext Context{this, RemovedIndices, EBuzzzHiveMutationType::Remove};
        PostHiveResize.Broadcast(Context);

        UBeeepMessageSubsystem::Get(this)->BroadcastMessage(Tag_BuzzzEvent_HiveResize, FInstancedStruct::Make(Context));
    }

    if (AddedIndices.Num() > 0)
    {
        const FBuzzzHiveMutationContext Context{this, AddedIndices, EBuzzzHiveMutationType::Add};
        PostHiveResize.Broadcast(Context);

        UBeeepMessageSubsystem::Get(this)->BroadcastMessage(Tag_BuzzzEvent_HiveResize, FInstancedStruct::Make(Context));
    }

    return true;
}


FBuzzzAssignmentContext UBuzzzContainer::ClearCell_Implementation(const int32& Index,
                                                                  FBuzzzAssignmentContext& OutContext)
{
    OutContext.Reset();
    OutContext.TargetIndex = Index;
    OutContext.UpcomingStackCount = 0;
    OutContext.TargetContainer = this;

    if (!CheckIndexIsValid(Index))
    {
        OutContext.State = EBuzzzExecutionState::Failed;
        return OutContext;
    }

    AssignCell(OutContext);
    return OutContext;
}


FBuzzzAssignmentContext UBuzzzContainer::AssignCell_Implementation(FBuzzzAssignmentContext& Context)
{
    FScopeLock ScopeLock(&ContainerCS);

#pragma region Validation

    if (Context.State == EBuzzzExecutionState::None)
    {
        Context.State = EBuzzzExecutionState::Executing;
    }

    // Make Sure TargetContainer is Set
    Context.TargetContainer = this;

    // Check Index Valid
    if (!CheckIndexIsValid(Context.TargetIndex))
    {
        Context.State = EBuzzzExecutionState::Failed;
    }
    // Nothing Changed
    else if (Hive.Cells[Context.TargetIndex].Item == Context.UpcomingInstance
        && Hive.Cells[Context.TargetIndex].StackCount == Context.UpcomingStackCount)
    {
        Context.State = EBuzzzExecutionState::Failed;
    }

    // StackCount Should be Positive ? Should Be?
    check(Context.UpcomingStackCount >= 0);
    if (Context.UpcomingStackCount < 0)
    {
        Context.State = EBuzzzExecutionState::Failed;
    }

    // if Empty Instance, StackCount Should Be 0
    if (!IsValid(Context.UpcomingInstance))
    {
        check(Context.UpcomingStackCount == 0);
    }

    // CheckCompatible
    if (IsValid(Context.UpcomingInstance) && !CheckItemCompatible(Context.UpcomingInstance))
    {
        Context.State = EBuzzzExecutionState::Failed;
    }

    // Fill up OutContext, Cache Previous Info
    {
        Context.PreviousInstance = Hive.Cells[Context.TargetIndex].Item;
        Context.PreviousStackCount = Hive.Cells[Context.TargetIndex].StackCount;
    }
#pragma endregion

    // Handle Failed
    if (Context.State == EBuzzzExecutionState::Failed)
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
        Hive.Cells[Context.TargetIndex].Item = Context.UpcomingInstance;
        Hive.Cells[Context.TargetIndex].StackCount = Context.UpcomingStackCount;
    }

    // Set Upcoming Replication
    if (IsValid(Context.UpcomingInstance))
    {
        check(IsUsingRegisteredSubObjectList())
        {
            AddReplicatedSubObject(Context.UpcomingInstance, HiveReplicateCondition);
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
        MarkIndexDirty(Context.TargetIndex);
    }

    // Mark as Success
    {
        Context.State = EBuzzzExecutionState::Success;
    }


    // Server Post Change Callback
    {
        PostCellChange.Broadcast(Context);
    }

    // Global Listener
    {
        UBeeepMessageSubsystem::Get(this)->BroadcastMessage(
            Tag_BuzzzEvent_CellMutation,
            FInstancedStruct::Make<const FBuzzzAssignmentContext>(Context));
    }

    return Context;
}

void UBuzzzContainer::InitializeComponent()
{
    Hive.ReceiveRemoteHiveMutation.AddWeakLambda(
        this, [this](const TArray<int32>& Indices, const EBuzzzHiveMutationType Type)
        {
            Client_ReceiveHiveMutation.Broadcast({this, Indices, Type});
        });

    if (GetOwner()->HasAuthority())
    {
        {
            PostCellChange.AddDynamic(this, &UBuzzzContainer::Internal_HandlePostCellChanged);
            PostHiveResize.AddDynamic(this, &UBuzzzContainer::Internal_HandlePostHiveResize);
        }

        OnInitialization();
    }

    Super::InitializeComponent();
}

void UBuzzzContainer::TickComponent(const float DeltaTime, const enum ELevelTick TickType,
                                    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if ((GetNetMode() == NM_Standalone)
        || GetNetMode() == NM_ListenServer
    )
    {
        Internal_Locally_TrySubmitMutationInfoToClient();
    }

    {
        Internal_Batched_RemovedIndices.Reset();
        Internal_Batched_AddedIndices.Reset();
        Internal_Batched_ChangedIndices.Reset();
        Internal_MayBeDisconnected_Instances.Reset();
    }
}


void UBuzzzContainer::BeginPlay()
{
    if (GetOwner()->HasAuthority())
    {
        Resize(InitialCapacity);
    }

    Super::BeginPlay();
}

void UBuzzzContainer::BeginDestroy()
{
    Super::BeginDestroy();
}

void UBuzzzContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    FDoRepLifetimeParams Params;
    Params.Condition = HiveReplicateCondition;
    Params.bIsPushBased = false;
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Hive, Params);

    constexpr FDoRepLifetimeParams ContainerGuidParams;
    Params.Condition = COND_InitialOnly;
    Params.bIsPushBased = true;
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ContainerGuid, ContainerGuidParams);
}

void UBuzzzContainer::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
    Super::PreReplication(ChangedPropertyTracker);
}

void UBuzzzContainer::OnInitialization_Implementation()
{
}
