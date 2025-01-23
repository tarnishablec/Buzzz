// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Presets/BuzzzInstance_UNIQUE.h"

#include "Core/Container/BuzzzContainer.h"
#include "Helpers/BuzzzAction_WaitForContainerOperation.h"
#include "Helpers/BuzzzAction_WaitForInstanceDisconnect.h"
#include "Helpers/BuzzzSharedTypes.h"
#include "Misc/EngineVersionComparison.h"

AActor* UBuzzzInstance_UNIQUE::GetOwnerActor_Implementation() const
{
    return GetTypedOuter<AActor>();
}

UBuzzzContainer* UBuzzzInstance_UNIQUE::GetOwnerContainer() const
{
    return Cast<UBuzzzContainer>(GetOuter());
}

void UBuzzzInstance_UNIQUE::HandlePutInAction(const FBuzzzCellAssignmentContext& Context)
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

void UBuzzzInstance_UNIQUE::HandleDisconnectAction(UBuzzzInstance* ItemInstance,
                                                       const UBuzzzContainer* Container)
{
    // Might Be Moved among One Same Container
    if (IsValid(Container))
    {
        this->ChangeOwnerContainer(nullptr);
    }
}

void UBuzzzInstance_UNIQUE::ChangeOwnerContainer(UBuzzzContainer* NewContainer)
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

void UBuzzzInstance_UNIQUE::BeginDestroy()
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

void UBuzzzInstance_UNIQUE::Initialize_Implementation()
{
    WaitDisconnectAction = UBuzzzAction_WaitForInstanceDisconnect::WaitForInstanceDisconnect(this);
    WaitDisconnectAction->Triggered.AddDynamic(this, &UBuzzzInstance_UNIQUE::HandleDisconnectAction);
    WaitDisconnectAction->Activate();

    WaitPuInAction = UBuzzzAction_WaitForContainerOperation::WaitForPutInContainer(this);
    WaitPuInAction->Triggered.AddDynamic(this, &UBuzzzInstance_UNIQUE::HandlePutInAction);
    WaitPuInAction->Activate();

    Super::Initialize_Implementation();
}
