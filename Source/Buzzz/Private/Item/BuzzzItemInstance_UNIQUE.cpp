// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Item/BuzzzItemInstance_UNIQUE.h"

#include "Helpers/BuzzzAction_WaitForContainerOperation.h"
#include "Item/BuzzzItemDefinition.h"


UBuzzzItemInstance* UBuzzzItemInstance_UNIQUE::MakeInstance_Implementation(
    const UBuzzzItemDefinition* InDefinition, AActor* Instigator) const
{
    check(IsValid(InDefinition) && IsValid(Instigator));

    const auto Instance = NewObject<UBuzzzItemInstance_UNIQUE>(Instigator, InDefinition->InstanceClass);
    Instance->Definition = InDefinition;
    return Instance;
}

UBuzzzContainer* UBuzzzItemInstance_UNIQUE::GetOwnerContainer() const
{
    return Cast<UBuzzzContainer>(GetOuter());
}

void UBuzzzItemInstance_UNIQUE::OnAssignAction(const FBuzzzOperationContext& Context)
{
    if (IsValid(Context.TargetContainer))
    {
        this->LowLevelRename(this->GetFName(), Context.TargetContainer);
    }
}

void UBuzzzItemInstance_UNIQUE::OnRemoveAction(const FBuzzzOperationContext& Context)
{
    if (IsValid(Context.FromContainer))
    {
        this->LowLevelRename(this->GetFName(), GetTransientPackage());
    }
}

void UBuzzzItemInstance_UNIQUE::InitializeInstance()
{
    AssignAction = UBuzzzAction_WaitForContainerOperation::WaitForAssignToCell(this);
    AssignAction->Triggered.AddDynamic(this, &UBuzzzItemInstance_UNIQUE::OnAssignAction);
    AssignAction->Activate();

    RemoveAction = UBuzzzAction_WaitForContainerOperation::WaitForRemoveFromCell(this);
    RemoveAction->Triggered.AddDynamic(this, &UBuzzzItemInstance_UNIQUE::OnRemoveAction);
    RemoveAction->Activate();

    Super::InitializeInstance();
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
