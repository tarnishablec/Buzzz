// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Item/BuzzzItemInstance_SHARED.h"
#include "Item/BuzzzItemDefinition.h"

TMap<const UBuzzzItemDefinition*, UBuzzzItemInstance*> UBuzzzItemInstance_SHARED::InstanceMap = {};

UBuzzzItemInstance_SHARED::UBuzzzItemInstance_SHARED()
{
}

UBuzzzItemInstance* UBuzzzItemInstance_SHARED::MakeInstance_Implementation(
    const UBuzzzItemDefinition* InDefinition, AActor* Instigator) const
{
    check(IsValid(InDefinition));

    const auto ExistInstancePtr = InstanceMap.Find(InDefinition);

    if (ExistInstancePtr)
    {
        return *ExistInstancePtr;
    }

    const auto FreshInstance = NewObject<UBuzzzItemInstance_SHARED>(GetTransientPackage(), InDefinition->InstanceClass);
    FreshInstance->Definition = InDefinition;
    InstanceMap.Add(InDefinition, FreshInstance);

    FreshInstance->InitializeInstance();
    
    return FreshInstance;
}
