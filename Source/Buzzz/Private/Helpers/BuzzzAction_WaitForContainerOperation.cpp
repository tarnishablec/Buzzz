// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Helpers/BuzzzAction_WaitForContainerOperation.h"

#include "Container/BuzzzSubsystem.h"
#include "Item/BuzzzItemInstance.h"

void UBuzzzAction_WaitForContainerOperation::Activate()
{
    Super::Activate();
    const auto BuzzzSubsystem = ItemInstance->GetWorld()->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
    if (BuzzzSubsystem)
    {
        BuzzzSubsystem->ReceivedContainerMutation.AddDynamic(
            this, &UBuzzzAction_WaitForContainerOperation::HandleReceivedContainerMutation);
    }
}

UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForAssignToCell(
    UBuzzzItemInstance* ItemInstance)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->ItemInstance = ItemInstance;
    Action->OperationMode = EOperationMode::Assign;
    return Action;
}

UBuzzzAction_WaitForContainerOperation* UBuzzzAction_WaitForContainerOperation::WaitForRemoveFromCell(
    UBuzzzItemInstance* ItemInstance)
{
    const auto Action = NewObject<UBuzzzAction_WaitForContainerOperation>();
    Action->ItemInstance = ItemInstance;
    Action->OperationMode = EOperationMode::Remove;
    return Action;
}


// ReSharper disable once CppMemberFunctionMayBeConst
void UBuzzzAction_WaitForContainerOperation::HandleReceivedContainerMutation(
    const FBuzzzOperationContext& Context)
{
    if (OperationMode == EOperationMode::Assign)
    {
        if (Context.UpcomingInstance == ItemInstance)
        {
            Triggered.Broadcast(Context);
        }
    }

    if (OperationMode == EOperationMode::Remove)
    {
        if (Context.PreviousInstance == ItemInstance)
        {
            Triggered.Broadcast(Context);
        }
    }
}

void UBuzzzAction_WaitForContainerOperation::Cancel()
{
    Super::Cancel();

    if (ItemInstance)
    {
        if (const auto World = ItemInstance->GetWorld())
        {
            const auto BuzzzSubsystem = World->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
            if (BuzzzSubsystem)
            {
                BuzzzSubsystem->ReceivedContainerMutation.RemoveAll(this);
            }
        }
    }
}
