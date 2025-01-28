// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Buzzz/Presets/BuzzzItem_UNIQUE.h"

#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Helpers/BuzzzAction_WaitForItemRemovalOrAddition.h"

void UBuzzzItem_UNIQUE::BeginDestroy()
{
    Super::BeginDestroy();
}

void UBuzzzItem_UNIQUE::Initialize()
{
    if (bInitialized)
    {
        return;
    }

    InitializeFragments();

    AdditionAction = UBuzzzAction_WaitForItemRemovalOrAddition::WaitForAddition(this, this);
    AdditionAction->Triggered.AddDynamic(this, &UBuzzzItem_UNIQUE::HandleAdditionToContainer);
    AdditionAction->Activate();

    RemovalAction = UBuzzzAction_WaitForItemRemovalOrAddition::WaitForRemoval(this, this);
    RemovalAction->Triggered.AddDynamic(this, &UBuzzzItem_UNIQUE::HandleRemovalFromContainer);
    RemovalAction->Activate();

    OnInitialization();
    bInitialized = true;
    PostInitialized();
}

void UBuzzzItem_UNIQUE::PreDemolish_Implementation()
{
    Super::PreDemolish_Implementation();

    AdditionAction->Cancel();
    RemovalAction->Cancel();
}

void UBuzzzItem_UNIQUE::HandleAdditionToContainer(const FBuzzzItemTransferContext& Context)
{
    const auto Container = Context.Container;
    LowLevelRename(GetFName(), static_cast<UObject*>(Container));
}

void UBuzzzItem_UNIQUE::HandleRemovalFromContainer(const FBuzzzItemTransferContext& Context)
{
    const auto Container = Context.Container;
    Container->RemoveReplicatedSubObject(this);

    TArray<UObject*> NetObjList{};
    this->GetSubobjectsWithStableNamesForNetworking(NetObjList);
    for (auto&& NetSubObject : NetObjList)
    {
        Container->RemoveReplicatedSubObject(NetSubObject);
    }

    if (Context.Container->IsValidLowLevelFast())
    {
        LowLevelRename(GetFName(), GetTransientPackage());
    }
}
