// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Item/BuzzzItemInstance_UNIQUE.h"

#include "Helpers/BuzzzAction_WaitForContainerOperation.h"
#include "Helpers/BuzzzSharedTypes.h"
#include "Item/BuzzzItemDefinition.h"
#include "Misc/EngineVersionComparison.h"

UBuzzzItemInstance* UBuzzzItemInstance_UNIQUE::MakeInstance_Implementation(
    const UBuzzzItemDefinition* InDefinition, AActor* Instigator) const
{
    check(IsValid(InDefinition) && IsValid(Instigator));

    const auto Instance = NewObject<UBuzzzItemInstance_UNIQUE>(Instigator, InDefinition->InstanceClass);
    Instance->Definition = InDefinition;

    Instance->InitializeInstance();
    return Instance;
}

UBuzzzContainer* UBuzzzItemInstance_UNIQUE::GetOwnerContainer() const
{
    return Cast<UBuzzzContainer>(GetOuter());
}

void UBuzzzItemInstance_UNIQUE::OnAssignAction(const FBuzzzCellAssignmentContext& Context)
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

void UBuzzzItemInstance_UNIQUE::OnRemoveAction(const FBuzzzCellAssignmentContext& Context)
{
    // Might Be Moved among One Same Container
    if (IsValid(Context.TargetContainer)
        && IsValid(GetOwnerContainer())
    )
    {
        this->ChangeOwnerContainer(nullptr);
    }
}

void UBuzzzItemInstance_UNIQUE::ChangeOwnerContainer(UBuzzzContainer* NewContainer)
{
    if (IsValid(NewContainer))
    {
        this->Rename(nullptr, NewContainer
#if UE_VERSION_OLDER_THAN(5, 5, 0)
                     ,REN_ForceNoResetLoaders
#endif
        );
    }
    else
    {
        this->Rename(nullptr, GetTransientPackage()
#if UE_VERSION_OLDER_THAN(5, 5, 0)
                     ,REN_ForceNoResetLoaders
#endif
        );
    }
}

void UBuzzzItemInstance_UNIQUE::BeginDestroy()
{
    Super::BeginDestroy();

    if (IsValid(AssignAction))
    {
        AssignAction->Cancel();
    }
    if (IsValid(RemoveAction))
    {
        RemoveAction->Cancel();
    }
}

void UBuzzzItemInstance_UNIQUE::InitializeInstance_Implementation()
{
    RemoveAction = UBuzzzAction_WaitForContainerOperation::WaitForClearedFromCell(this);
    RemoveAction->Triggered.AddDynamic(this, &UBuzzzItemInstance_UNIQUE::OnRemoveAction);
    RemoveAction->Activate();

    AssignAction = UBuzzzAction_WaitForContainerOperation::WaitForAssignToCell(this);
    AssignAction->Triggered.AddDynamic(this, &UBuzzzItemInstance_UNIQUE::OnAssignAction);
    AssignAction->Activate();

    Super::InitializeInstance_Implementation();
}
