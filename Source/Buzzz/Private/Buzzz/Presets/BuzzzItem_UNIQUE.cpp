// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Buzzz/Presets/BuzzzItem_UNIQUE.h"

#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Helpers/BuzzzAction_WaitForContainerOperation.h"
#include "Buzzz/Helpers/BuzzzAction_WaitForInstanceDisconnect.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "Misc/EngineVersionComparison.h"

AActor* UBuzzzItem_UNIQUE::GetOwnerActor_Implementation() const
{
    return GetTypedOuter<AActor>();
}

UBuzzzContainer* UBuzzzItem_UNIQUE::GetOwnerContainer() const
{
    return Cast<UBuzzzContainer>(GetOuter());
}

void UBuzzzItem_UNIQUE::HandlePutInAction(const FBuzzzCellAssignmentContext& Context)
{
    // Might Be Moved among One Same Container
    if (IsValid(Context.TargetContainer)
        && Context.TargetContainer != Context.FromContainer
        && GetOwnerContainer() != Context.TargetContainer
    )
    {
        this->ChangeOwnerContainer(Context.TargetContainer);
    }
}

void UBuzzzItem_UNIQUE::HandleDisconnectAction(UBuzzzItem* Item,
                                                       const UBuzzzContainer* Container)
{
    // Might Be Moved among One Same Container
    if (IsValid(Container))
    {
        this->ChangeOwnerContainer(nullptr);
    }
}

void UBuzzzItem_UNIQUE::ChangeOwnerContainer(UBuzzzContainer* NewContainer)
{
    if (IsValid(NewContainer))
    {
        this->LowLevelRename(GetFName(), NewContainer);
        // this->Rename(nullptr, NewContainer
#if UE_VERSION_OLDER_THAN(5, 5, 0)
        // ,REN_ForceNoResetLoaders
#endif
        // );
    }
    else
    {
        this->LowLevelRename(GetFName(), GetTransientPackage());
        // this->Rename(nullptr, GetTransientPackage()
#if UE_VERSION_OLDER_THAN(5, 5, 0)
        // ,REN_ForceNoResetLoaders
#endif
        // );
    }
}

void UBuzzzItem_UNIQUE::BeginDestroy()
{
    Super::BeginDestroy();

    if (IsValid(WaitPuInAction))
    {
        WaitPuInAction->Cancel();
    }
    if (IsValid(WaitDisconnectAction))
    {
        WaitDisconnectAction->Cancel();
    }
}

void UBuzzzItem_UNIQUE::Initialize_Implementation()
{
    WaitDisconnectAction = UBuzzzAction_WaitForInstanceDisconnect::WaitForInstanceDisconnect(this);
    WaitDisconnectAction->Triggered.AddDynamic(this, &UBuzzzItem_UNIQUE::HandleDisconnectAction);
    WaitDisconnectAction->Activate();

    WaitPuInAction = UBuzzzAction_WaitForContainerOperation::WaitForPutInContainer(this);
    WaitPuInAction->Triggered.AddDynamic(this, &UBuzzzItem_UNIQUE::HandlePutInAction);
    WaitPuInAction->Activate();

    Super::Initialize_Implementation();
}
