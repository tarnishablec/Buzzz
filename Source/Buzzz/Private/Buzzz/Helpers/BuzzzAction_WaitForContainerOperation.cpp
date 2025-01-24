// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Buzzz/Helpers/BuzzzAction_WaitForContainerOperation.h"
#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Subsystem/BuzzzSubsystem.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "Buzzz/Core/Item/BuzzzInstance.h"


UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForAssignToCell(
    UBuzzzInstance* ItemInstance)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->TargetItemInstance = ItemInstance;
    Action->OperationMode = EOperationMode::Assign;
    return Action;
}

UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForClearedFromCell(
    UBuzzzInstance* ItemInstance)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->TargetItemInstance = ItemInstance;
    Action->OperationMode = EOperationMode::Clear;
    return Action;
}

UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForPutInContainer(
    UBuzzzInstance* ItemInstance)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->TargetItemInstance = ItemInstance;
    Action->OperationMode = EOperationMode::PutIn;
    return Action;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBuzzzAction_WaitForContainerOperation::HandleReceivedContainerMutation(
    const FBuzzzCellAssignmentContext& Context)
{
    if (OperationMode == EOperationMode::Clear)
    {
        if (Context.PreviousInstance == TargetItemInstance
            && Context.UpcomingInstance == nullptr
            && Context.FromContainer == nullptr)
        {
            Triggered.Broadcast(Context);
        }
    }

    if (OperationMode == EOperationMode::Assign)
    {
        if (Context.UpcomingInstance == TargetItemInstance)
        {
            Triggered.Broadcast(Context);
        }
    }

    if (OperationMode == EOperationMode::PutIn)
    {
        if (Context.UpcomingInstance == TargetItemInstance
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

    const auto BuzzzSubsystem = TargetItemInstance->GetWorld()->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
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

    if (TargetItemInstance)
    {
        Triggered.RemoveAll(TargetItemInstance);

        if (const auto World = TargetItemInstance->GetWorld())
        {
            const auto BuzzzSubsystem = World->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
            if (IsValid(BuzzzSubsystem))
            {
                BuzzzSubsystem->ReceiveContainerCellMutation.RemoveAll(this);
            }
        }
    }
}
