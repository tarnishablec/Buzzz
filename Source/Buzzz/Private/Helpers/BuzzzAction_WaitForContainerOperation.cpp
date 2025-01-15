// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Helpers/BuzzzAction_WaitForContainerOperation.h"

#include "Container/BuzzzSubsystem.h"
#include "Item/BuzzzItemInstance.h"


UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForAssignToCell(
    UBuzzzItemInstance* ItemInstance)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->TargetItemInstance = ItemInstance;
    Action->OperationMode = EOperationMode::Assign;
    return Action;
}

UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForRemoveFromCell(
    UBuzzzItemInstance* ItemInstance)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->TargetItemInstance = ItemInstance;
    Action->OperationMode = EOperationMode::Remove;
    return Action;
}

UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForPutInContainer(
    UBuzzzItemInstance* ItemInstance, UBuzzzContainer* Container)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->TargetItemInstance = ItemInstance;
    Action->OperationMode = EOperationMode::PutIn;
    Action->TargetContainer = Container;

    return Action;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBuzzzAction_WaitForContainerOperation::HandleReceivedContainerMutation(
    const FBuzzzCellOperationContext& Context)
{
    if (OperationMode == EOperationMode::Assign)
    {
        if (Context.UpcomingInstance == TargetItemInstance)
        {
            Triggered.Broadcast(Context);
        }
    }

    if (OperationMode == EOperationMode::Remove)
    {
        if (Context.PreviousInstance == TargetItemInstance)
        {
            Triggered.Broadcast(Context);
        }
    }

    if (OperationMode == EOperationMode::PutIn)
    {
        if (Context.FromContainer != TargetContainer && Context.TargetContainer == TargetContainer)
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
        EOperationMode::Remove)
    {
        BuzzzSubsystem->ReceivedContainerCellMutation.AddDynamic(
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
                BuzzzSubsystem->ReceivedContainerCellMutation.RemoveAll(this);
            }
        }
    }
}
