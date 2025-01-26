// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Subsystem/BuzzzManager.h"

#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "StructUtils/InstancedStruct.h"

UE_DEFINE_GAMEPLAY_TAG(Tag_BuzzzEvent_ItemDisconnect, "BuzzzEvent.ItemDisconnect");

// Sets default values
ABuzzzManager::ABuzzzManager()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    SetHidden(true);
    SetReplicatingMovement(false);
    SetCanBeDamaged(false);
    bEnableAutoLODGeneration = false;
}

void ABuzzzManager::ReceivedCellMutation_Implementation(const FBuzzzCellAssignmentContext& Context)
{
}

// Called when the game starts or when spawned
void ABuzzzManager::BeginPlay()
{
    Super::BeginPlay();

    auto Callback = [this](FGameplayTag, const FInstancedStruct& Payload)
    {
        if (const auto* Context = Payload.GetPtr<FBuzzzCellAssignmentContext>())
        {
            // TODO: Should Have Some Bug
            if (IsValid(Context->TargetContainer) && IsValid(Context->PreviousInstance))
            {
                Recycler.Add(Context->PreviousInstance, Context->TargetContainer);
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

    if (Recycler.Num() > 0)
    {
        for (auto&& Conditional : Recycler)
        {
            if (!Conditional.Value->CheckItemOwned(Conditional.Key))
            {
                UBeeepMessageSubsystem::Get(this)->BroadcastMessage(Tag_BuzzzEvent_ItemDisconnect);
            }
        }

        Recycler.Reset();
    }
}

void ABuzzzManager::BeginDestroy()
{
    // UBeeepMessageSubsystem::Get(this)->UnregisterListener(CellMutationListenerHandle);
    Super::BeginDestroy();
}
