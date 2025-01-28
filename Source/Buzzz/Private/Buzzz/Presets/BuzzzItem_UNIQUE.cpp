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

void UBuzzzItem_UNIQUE::PreKilled_Implementation()
{
    Super::PreKilled_Implementation();

    FBuzzzItemTransferContext Context;
    Context.Container = Cast<UBuzzzContainer>(this->GetOuter());
    Context.Item = this;
    Context.TransferType = EBuzzzItemTransferType::Removal;
    HandleRemovalFromContainer(Context);

    AdditionAction->Cancel();
    RemovalAction->Cancel();
}

void UBuzzzItem_UNIQUE::Kill()
{
    Super::Kill();
}

UBuzzzContainer* UBuzzzItem_UNIQUE::GetOwnerContainer() const
{
    return Cast<UBuzzzContainer>(GetOuter());
}

void UBuzzzItem_UNIQUE::HandleAdditionToContainer(const FBuzzzItemTransferContext& Context)
{
    const auto Container = Context.Container;
    if (Container != GetOuter())
    {
        LowLevelRename(GetFName(), Container);
    }
}

void UBuzzzItem_UNIQUE::HandleRemovalFromContainer(const FBuzzzItemTransferContext& Context)
{
    const auto Container = Context.Container;
    
    if (Container->IsValidLowLevelFast())
    {
        LowLevelRename(GetFName(), GetTransientPackage());
    }
    //
    // Container->RemoveReplicatedSubObject(this);
    // TArray<UObject*> NetObjList{};
    // this->GetSubobjectsWithStableNamesForNetworking(NetObjList);
    // for (auto&& NetSubObject : NetObjList)
    // {
    //     Container->RemoveReplicatedSubObject(NetSubObject);
    // }
}
