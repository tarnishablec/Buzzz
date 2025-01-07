// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Item/BuzzzItemInstance_SHARED.h"
#include "Item/BuzzzItemDefinition.h"

TMap<const UBuzzzItemDefinition*, UBuzzzItemInstance_SHARED*> UBuzzzItemInstance_SHARED::InstanceMap = {};

UBuzzzItemInstance_SHARED::UBuzzzItemInstance_SHARED()
{
}

UBuzzzItemInstance* UBuzzzItemInstance_SHARED::MakeInstance_Implementation(
    const UBuzzzItemDefinition* InDefinition) const
{
    check(IsValid(InDefinition))

    const auto ExistInstance = InstanceMap.Find(InDefinition);
    if (ExistInstance != nullptr && IsValid(*ExistInstance))
    {
        return *ExistInstance;
    }

    const auto FreshInstance = NewObject<UBuzzzItemInstance_SHARED>(GetTransientPackage(), InDefinition->InstanceClass);
    InstanceMap.Add(InDefinition, FreshInstance);
    return FreshInstance;
}
