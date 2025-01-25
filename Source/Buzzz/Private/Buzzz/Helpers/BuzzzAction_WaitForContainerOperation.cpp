// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Buzzz/Helpers/BuzzzAction_WaitForContainerOperation.h"
#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Subsystem/BuzzzSubsystem.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "Buzzz/Core/Item/BuzzzItem.h"


UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForAssignToCell(
    UBuzzzItem* Item)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->TargetItem = Item;
    Action->OperationMode = EOperationMode::Assign;
    return Action;
}

UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForClearedFromCell(
    UBuzzzItem* Item)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->TargetItem = Item;
    Action->OperationMode = EOperationMode::Clear;
    return Action;
}

UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForPutInContainer(
    UBuzzzItem* Item)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->TargetItem = Item;
    Action->OperationMode = EOperationMode::PutIn;
    return Action;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBuzzzAction_WaitForContainerOperation::HandleReceivedContainerMutation(
    const FBuzzzCellAssignmentContext& Context)
{
    if (OperationMode == EOperationMode::Clear)
    {
        if (Context.PreviousInstance == TargetItem
            && Context.UpcomingInstance == nullptr
            && Context.FromContainer == nullptr)
        {
            Triggered.Broadcast(Context);
        }
    }

    if (OperationMode == EOperationMode::Assign)
    {
        if (Context.UpcomingInstance == TargetItem)
        {
            Triggered.Broadcast(Context);
        }
    }

    if (OperationMode == EOperationMode::PutIn)
    {
        if (Context.UpcomingInstance == TargetItem
            && IsValid(Context.TargetContainer)
            && Context.FromContainer != Context.TargetContainer)
        {
            Triggered.Broadcast(Context);
        }
    }
}


void UBuzzzAction_WaitForContainerOperation::Activate()
{
    Super::Activate();

    const auto BuzzzSubsystem = TargetItem->GetWorld()->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
    check(IsValid(BuzzzSubsystem))

    if (OperationMode == EOperationMode::Assign || OperationMode == EOperationMode::PutIn || OperationMode ==
        EOperationMode::Clear)
    {
        BuzzzSubsystem->ReceiveContainerCellMutation.AddDynamic(
            this, &UBuzzzAction_WaitForContainerOperation::HandleReceivedContainerMutation);
    }
}

void UBuzzzAction_WaitForContainerOperation::Cancel()
{
    Super::Cancel();

    if (TargetItem)
    {
        Triggered.RemoveAll(TargetItem);

        if (const auto World = TargetItem->GetWorld())
        {
            const auto BuzzzSubsystem = World->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
            if (IsValid(BuzzzSubsystem))
            {
                BuzzzSubsystem->ReceiveContainerCellMutation.RemoveAll(this);
            }
        }
    }
}
