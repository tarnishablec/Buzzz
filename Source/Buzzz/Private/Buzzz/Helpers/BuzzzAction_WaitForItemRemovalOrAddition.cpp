// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Helpers/BuzzzAction_WaitForItemRemovalOrAddition.h"

#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Buzzz/Subsystem/BuzzzManager.h"

UBuzzzAction_WaitForItemRemovalOrAddition* UBuzzzAction_WaitForItemRemovalOrAddition::WaitForRemoval(
    const UObject* WorldContextObject,
    UBuzzzItem* Item
)
{
    const auto Action = NewObject<UBuzzzAction_WaitForItemRemovalOrAddition>();
    Action->TargetItem = Item;
    Action->ActionMode = EAdditionOrRemoval::Removal;
    Action->WorldPtr = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    return Action;
}

UBuzzzAction_WaitForItemRemovalOrAddition* UBuzzzAction_WaitForItemRemovalOrAddition::WaitForAddition(
    const UObject* WorldContextObject, UBuzzzItem* Item)
{
    const auto Action = NewObject<UBuzzzAction_WaitForItemRemovalOrAddition>();
    Action->TargetItem = Item;
    Action->ActionMode = EAdditionOrRemoval::Addition;
    Action->WorldPtr = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    return Action;
}

void UBuzzzAction_WaitForItemRemovalOrAddition::Activate()
{
    Super::Activate();

    FGameplayTag EventTag;

    switch (ActionMode)
    {
    case EAdditionOrRemoval::Addition: EventTag = Tag_BuzzzEvent_ItemAddition;
        break;
    case EAdditionOrRemoval::Removal: EventTag = Tag_BuzzzEvent_ItemRemoval;
        break;
    default: ;
    }

    if (WorldPtr.IsValid() && EventTag.IsValid())
    {
        UBeeepMessageSubsystem::Get(WorldPtr.Get())->RegisterListener(
            {
                EventTag,
                EBeeepChannelMatchMode::ExactMatch,
                [this](FGameplayTag Channel, const FInstancedStruct& Payload)
                {
                    if (const auto Context = Payload.GetPtr<FBuzzzItemTransferContext>())
                    {
                        if (Context->Item == TargetItem)
                        {
                            Triggered.Broadcast(*Context);
                        }
                    }
                }
            }, ListenerHandle);
    }
}

void UBuzzzAction_WaitForItemRemovalOrAddition::SetReadyToDestroy()
{
    if (const auto World = WorldPtr.Get())
    {
        UBeeepMessageSubsystem::Get(World)->UnregisterListener(ListenerHandle);
    }

    if (TargetItem.IsValid())
    {
        Triggered.RemoveAll(TargetItem.Get());
    }

    Super::SetReadyToDestroy();
}
