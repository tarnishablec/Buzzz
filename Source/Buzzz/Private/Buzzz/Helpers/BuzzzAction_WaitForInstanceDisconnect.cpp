// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Helpers/BuzzzAction_WaitForInstanceDisconnect.h"

#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Buzzz/Subsystem/BuzzzManager.h"

UBuzzzAction_WaitForInstanceDisconnect* UBuzzzAction_WaitForInstanceDisconnect::WaitForInstanceDisconnect(
    UObject* WorldContextObject,
    UBuzzzItem* Item
)
{
    const auto Action = NewObject<UBuzzzAction_WaitForInstanceDisconnect>();
    Action->TargetItem = Item;
    Action->WorldPtr = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    return Action;
}

void UBuzzzAction_WaitForInstanceDisconnect::Activate()
{
    Super::Activate();

    if (WorldPtr.IsValid())
    {
        UBeeepMessageSubsystem::Get(WorldPtr.Get())->RegisterListener(
            {
                Tag_BuzzzEvent_ItemDisconnect,
                EBeeepChannelMatchMode::ExactMatch,
                [this](FGameplayTag Channel, const FInstancedStruct& Payload)
                {
                    if (const auto Context = Payload.GetPtr<FBuzzzItemDisconnectContext>())
                    {
                        if (Context->Item == TargetItem)
                        {
                            Triggered.Broadcast(*Context);
                        }
                    }
                }
            }, WaitForDisconnectListenerHandle);
    }
}

void UBuzzzAction_WaitForInstanceDisconnect::SetReadyToDestroy()
{
    if (const auto World = WorldPtr.Get())
    {
        UBeeepMessageSubsystem::Get(World)->UnregisterListener(WaitForDisconnectListenerHandle);
    }

    if (TargetItem.IsValid())
    {
        Triggered.RemoveAll(TargetItem.Get());
    }

    Super::SetReadyToDestroy();
}
