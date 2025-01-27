// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Subsystem/BuzzzManager.h"

#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "StructUtils/InstancedStruct.h"

UE_DEFINE_GAMEPLAY_TAG(Tag_BuzzzEvent_ItemRemoval, "BuzzzEvent.ItemRemoval");
UE_DEFINE_GAMEPLAY_TAG(Tag_BuzzzEvent_ItemAddition, "BuzzzEvent.ItemAddition");

// Sets default values
ABuzzzManager::ABuzzzManager()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    SetHidden(true);
    SetReplicatingMovement(false);
    SetCanBeDamaged(false);
    bEnableAutoLODGeneration = false;
}

AActor* ABuzzzManager::TryAchieveItemOwner(UBuzzzItem* Item)
{
    return this;
}

// Called when the game starts or when spawned
void ABuzzzManager::BeginPlay()
{
    Super::BeginPlay();
}

void ABuzzzManager::BeginDestroy()
{
    Super::BeginDestroy();
}

void ABuzzzManager::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    auto Callback = [this](FGameplayTag, const FInstancedStruct& Payload)
    {
        if (const auto* Context = Payload.GetPtr<FBuzzzCellAssignmentContext>())
        {
            if (IsValid(Context->TargetContainer))
            {
                if (IsValid(Context->PreviousInstance))
                {
                    Recycler.Throw(Context->PreviousInstance, Context->TargetContainer);
                }

                if (IsValid(Context->UpcomingInstance))
                {
                    Recycler.Pick(Context->UpcomingInstance, Context->TargetContainer);
                }
            }
        }
    };

    UBeeepMessageSubsystem::Get(this)->RegisterListener({
                                                            Tag_BuzzzEvent_CellMutation,
                                                            EBeeepChannelMatchMode::ExactMatch,
                                                            Callback
                                                        }, CellMutationListenerHandle);
}

// Called every frame
void ABuzzzManager::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    for (auto&& Entry : Recycler.ContainerMap)
    {
        const auto Container = Entry.Key;
        const auto ItemSet = Entry.Value.ItemCountMap;
        if (Container.IsValid())
        {
            for (auto&& ItemCountEntry : ItemSet)
            {
                const auto Item = ItemCountEntry.Key;
                if (Item.IsValid())
                {
                    if (ItemCountEntry.Value < 0)
                    {
                        UBeeepMessageSubsystem::Get(this)->BroadcastMessage(
                            Tag_BuzzzEvent_ItemRemoval, FInstancedStruct::Make(
                                FBuzzzItemDisconnectContext{
                                    Item.Get(), Container.Get()
                                }));
                    }

                    if (ItemCountEntry.Value > 0)
                    {
                        UBeeepMessageSubsystem::Get(this)->BroadcastMessage(
                            Tag_BuzzzEvent_ItemAddition, FInstancedStruct::Make(
                                FBuzzzItemDisconnectContext{
                                    Item.Get(), Container.Get()
                                }));
                    }
                }
            }
        }
    }

    Recycler.Reset();
}


void ABuzzzManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UBeeepMessageSubsystem::Get(this)->UnregisterListener(CellMutationListenerHandle);
    Super::EndPlay(EndPlayReason);
}
