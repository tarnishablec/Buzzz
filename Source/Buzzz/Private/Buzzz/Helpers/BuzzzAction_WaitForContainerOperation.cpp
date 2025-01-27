// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Buzzz/Helpers/BuzzzAction_WaitForContainerOperation.h"

#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Subsystem/BuzzzSubsystem.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "Buzzz/Core/Item/BuzzzItem.h"


UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForAssignToCell(
    UWorld* WorldContextObject,
    UBuzzzItem* Item)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->WorldPtr = WorldContextObject->GetWorld();
    Action->TargetItem = Item;
    Action->OperationMode = EOperationMode::Assign;
    return Action;
}

UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForClearedFromCell(
    UWorld* WorldContextObject,
    UBuzzzItem* Item)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->WorldPtr = WorldContextObject->GetWorld();
    Action->TargetItem = Item;
    Action->OperationMode = EOperationMode::Clear;
    return Action;
}

UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForPutInContainer(
    UWorld* WorldContextObject,
    UBuzzzItem* Item)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->WorldPtr = WorldContextObject->GetWorld();
    Action->TargetItem = Item;
    Action->OperationMode = EOperationMode::PutIn;
    return Action;
}


void UBuzzzAction_WaitForContainerOperation::Activate()
{
    Super::Activate();

    if (WorldPtr.IsValid())
    {
        if (const auto World = WorldPtr.Get())
        {
            TWeakObjectPtr WeakThis(this);
            auto Callback = [WeakThis](FGameplayTag, const FInstancedStruct& Payload)
            {
                const auto Context = Payload.GetPtr<const FBuzzzCellAssignmentContext>();
                if (!Context || !WeakThis.IsValid())
                {
                    return;
                }

                const auto StrongThis = WeakThis.Get();
                if (IsValid(StrongThis))
                {
                    if (StrongThis->OperationMode == EOperationMode::Clear)
                    {
                        if (Context->PreviousInstance == StrongThis->TargetItem
                            && Context->UpcomingInstance == nullptr
                            && Context->FromContainer == nullptr)
                        {
                            StrongThis->Triggered.Broadcast(*Context);
                        }
                    }

                    if (StrongThis->OperationMode == EOperationMode::Assign)
                    {
                        if (Context->UpcomingInstance == StrongThis->TargetItem)
                        {
                            StrongThis->Triggered.Broadcast(*Context);
                        }
                    }

                    if (StrongThis->OperationMode == EOperationMode::PutIn)
                    {
                        if (Context->UpcomingInstance == StrongThis->TargetItem
                            && IsValid(Context->TargetContainer)
                            && Context->FromContainer != Context->TargetContainer)
                        {
                            StrongThis->Triggered.Broadcast(*Context);
                        }
                    }
                }
            };

            UBeeepMessageSubsystem::Get(World)->RegisterListener({
                                                                     Tag_BuzzzEvent_CellMutation,
                                                                     EBeeepChannelMatchMode::ExactMatch, Callback
                                                                 }, *Handle);
        }
    }
}

void UBuzzzAction_WaitForContainerOperation::Cancel()
{
    Super::Cancel();

    if (TargetItem.IsValid())
    {
        Triggered.RemoveAll(TargetItem.Get());

        if (const auto World = WorldPtr.Get())
        {
            UBeeepMessageSubsystem::Get(World)->UnregisterListener(*Handle);
        }
    }
}
