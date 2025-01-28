// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Buzzz/Helpers/BuzzzAction_WaitForItemAssignment.h"

#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Subsystem/BuzzzSubsystem.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "Buzzz/Core/Item/BuzzzItem.h"


UBuzzzAction_WaitForItemAssignment* UBuzzzAction_WaitForItemAssignment::WaitForAssignToCell(
    const UWorld* WorldContextObject, UBuzzzItem* Item)
{
    const auto Action = NewObject<UBuzzzAction_WaitForItemAssignment>();
    Action->WorldPtr = WorldContextObject->GetWorld();
    Action->TargetItem = Item;
    Action->RegisterWithGameInstance(WorldContextObject);
    return Action;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBuzzzAction_WaitForItemAssignment::HandleMessage(FGameplayTag Channel, const FInstancedStruct& Payload)
{
    const auto Context = Payload.GetPtr<const FBuzzzAssignmentContext>();
    if (!Context)
    {
        return;
    }

    if (Context->UpcomingInstance == this->TargetItem)
    {
        this->Triggered.Broadcast(*Context);
    }
}


void UBuzzzAction_WaitForItemAssignment::Activate()
{
    Super::Activate();

    if (WorldPtr.IsValid())
    {
        if (const auto World = WorldPtr.Get())
        {
            FBeeepMessageListenerParams Params{
                Tag_BuzzzEvent_CellMutation,
                EBeeepChannelMatchMode::ExactMatch
            };

            Params.MessageReceived.BindDynamic(this, &UBuzzzAction_WaitForItemAssignment::HandleMessage);

            UBeeepMessageSubsystem::Get(World)->RegisterListener(
                Params, *Handle);
        }
    }
}

void UBuzzzAction_WaitForItemAssignment::Cancel()
{
    Super::Cancel();
}

void UBuzzzAction_WaitForItemAssignment::SetReadyToDestroy()
{
    if (TargetItem.IsValid())
    {
        Triggered.RemoveAll(TargetItem.Get());

        if (const auto World = WorldPtr.Get())
        {
            UBeeepMessageSubsystem::Get(World)->UnregisterListener(*Handle);
        }
    }
    Super::SetReadyToDestroy();
}
