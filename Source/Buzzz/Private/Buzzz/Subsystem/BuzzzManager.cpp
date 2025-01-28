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
    bNetLoadOnClient = false;
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

    FBeeepMessageListenerParams Params{
        Tag_BuzzzEvent_CellMutation,
        EBeeepChannelMatchMode::ExactMatch
    };

    Params.MessageReceived.BindDynamic(this, &ABuzzzManager::HandleCellMutationMessage);

    UBeeepMessageSubsystem::Get(this)->RegisterListener(Params, CellMutationListenerHandle);
}

// Called every frame
void ABuzzzManager::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    for (auto&& Entry : MutationCounter.ContainerMap)
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
                                FBuzzzItemTransferContext{
                                    Item.Get(),
                                    Container.Get(),
                                    EBuzzzItemTransferType::Removal
                                }));
                    }

                    if (ItemCountEntry.Value > 0)
                    {
                        UBeeepMessageSubsystem::Get(this)->BroadcastMessage(
                            Tag_BuzzzEvent_ItemAddition, FInstancedStruct::Make(
                                FBuzzzItemTransferContext{
                                    Item.Get(),
                                    Container.Get(),
                                    EBuzzzItemTransferType::Addition
                                }));
                    }
                }
            }
        }
    }

    MutationCounter.Reset();
}


void ABuzzzManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UBeeepMessageSubsystem::Get(this)->UnregisterListener(CellMutationListenerHandle);
    Super::EndPlay(EndPlayReason);
}

void ABuzzzManager::HandleCellMutationMessage(FGameplayTag Channel, const FInstancedStruct& Payload)
{
    if (const auto* Context = Payload.GetPtr<FBuzzzAssignmentContext>())
    {
        if (IsValid(Context->TargetContainer))
        {
            // We Should Throw First
            if (IsValid(Context->PreviousInstance))
            {
                MutationCounter.Throw(Context->PreviousInstance, Context->TargetContainer);
            }

            if (IsValid(Context->UpcomingInstance))
            {
                MutationCounter.Pick(Context->UpcomingInstance, Context->TargetContainer);
            }
        }
    }
}
