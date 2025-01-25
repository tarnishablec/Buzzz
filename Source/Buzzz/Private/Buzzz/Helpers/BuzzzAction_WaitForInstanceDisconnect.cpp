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
    
    const auto BuzzzSubsystem = TargetItem->GetWorld()->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
    check(IsValid(BuzzzSubsystem))

    BuzzzSubsystem->ReceiveInstanceDisconnect.AddDynamic(
        this, &UBuzzzAction_WaitForInstanceDisconnect::HandleReceivedInstanceDisconnect);
}

void UBuzzzAction_WaitForInstanceDisconnect::Cancel()
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
                BuzzzSubsystem->ReceiveInstanceDisconnect.RemoveAll(this);
            }
        }
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
