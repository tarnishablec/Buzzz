// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Item/BuzzzItemInstance_UNIQUE.h"

#include "Helpers/BuzzzAction_WaitForContainerOperation.h"
#include "Helpers/BuzzzAction_WaitForInstanceDisconnect.h"
#include "Helpers/BuzzzSharedTypes.h"
#include "Item/BuzzzItemDefinition.h"
#include "Misc/EngineVersionComparison.h"

UBuzzzItemInstance* UBuzzzItemInstance_UNIQUE::MakeInstance_Implementation(
    const UBuzzzItemDefinition* InDefinition,
    AActor* Instigator) const
{
    UObject* FinalInstigator = IsValid(Instigator) ? Cast<UObject>(Instigator) : GetTransientPackage();
    const auto Instance = NewObject<UBuzzzItemInstance_UNIQUE>(
        FinalInstigator,
        InDefinition->InstanceClass);
    Instance->Definition = InDefinition;
    return Instance;
}

AActor* UBuzzzItemInstance_UNIQUE::GetOwnerActor_Implementation() const
{
    return GetTypedOuter<AActor>();
}

UBuzzzContainer* UBuzzzItemInstance_UNIQUE::GetOwnerContainer() const
{
    return Cast<UBuzzzContainer>(GetOuter());
}

void UBuzzzItemInstance_UNIQUE::HandlePutInAction(const FBuzzzCellAssignmentContext& Context)
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

void UBuzzzItemInstance_UNIQUE::HandleDisconnectAction(UBuzzzItemInstance* ItemInstance,
                                                       const UBuzzzContainer* Container)
{
    // Might Be Moved among One Same Container
    if (IsValid(Container))
    {
        this->ChangeOwnerContainer(nullptr);
    }
}

void UBuzzzItemInstance_UNIQUE::ChangeOwnerContainer(UBuzzzContainer* NewContainer)
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

void UBuzzzItemInstance_UNIQUE::BeginDestroy()
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

void UBuzzzItemInstance_UNIQUE::Initialize_Implementation()
{
    WaitDisconnectAction = UBuzzzAction_WaitForInstanceDisconnect::WaitForInstanceDisconnect(this);
    WaitDisconnectAction->Triggered.AddDynamic(this, &UBuzzzItemInstance_UNIQUE::HandleDisconnectAction);
    WaitDisconnectAction->Activate();

    WaitPuInAction = UBuzzzAction_WaitForContainerOperation::WaitForPutInContainer(this);
    WaitPuInAction->Triggered.AddDynamic(this, &UBuzzzItemInstance_UNIQUE::HandlePutInAction);
    WaitPuInAction->Activate();

    Super::Initialize_Implementation();
}
