// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Helpers/BuzzzAction_WaitForInstanceDisconnect.h"

#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Buzzz/Subsystem/BuzzzSubsystem.h"

UBuzzzAction_WaitForInstanceDisconnect* UBuzzzAction_WaitForInstanceDisconnect::WaitForInstanceDisconnect(
    UBuzzzItem* Item)
{
    const auto Action = NewObject<UBuzzzAction_WaitForInstanceDisconnect>();
    Action->TargetItem = Item;
    return Action;
}

void UBuzzzAction_WaitForInstanceDisconnect::Activate()
{
    Super::Activate();
}

void UBuzzzAction_WaitForInstanceDisconnect::Cancel()
{
    Super::Cancel();

    if (TargetItem)
    {
        Triggered.RemoveAll(TargetItem);
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBuzzzAction_WaitForInstanceDisconnect::HandleReceivedInstanceDisconnect(UBuzzzItem* Item,
                                                                              const UBuzzzContainer* Container)
{
    if (Item == TargetItem)
    {
        Triggered.Broadcast(Item, Container);
    }
}
