// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Helpers/BuzzzAction_WaitForInstanceDisconnect.h"

#include "Item/BuzzzItemInstance.h"

UBuzzzAction_WaitForInstanceDisconnect* UBuzzzAction_WaitForInstanceDisconnect::WaitForInstanceDisconnect(
    UBuzzzItemInstance* ItemInstance)
{
    const auto Action = NewObject<UBuzzzAction_WaitForInstanceDisconnect>();
    Action->TargetItemInstance = ItemInstance;
    return Action;
}

void UBuzzzAction_WaitForInstanceDisconnect::Activate()
{
    Super::Activate();
    
    const auto BuzzzSubsystem = TargetItemInstance->GetWorld()->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
    check(IsValid(BuzzzSubsystem))

    BuzzzSubsystem->ReceivedInstanceDisconnect.AddDynamic(
        this, &UBuzzzAction_WaitForInstanceDisconnect::HandleReceivedInstanceDisconnect);
}

void UBuzzzAction_WaitForInstanceDisconnect::Cancel()
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
                BuzzzSubsystem->ReceivedInstanceDisconnect.RemoveAll(this);
            }
        }
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBuzzzAction_WaitForInstanceDisconnect::HandleReceivedInstanceDisconnect(UBuzzzItemInstance* ItemInstance,
                                                                              UBuzzzContainer* Container)
{
    if (ItemInstance == TargetItemInstance)
    {
        Triggered.Broadcast(ItemInstance, Container);
    }
}
