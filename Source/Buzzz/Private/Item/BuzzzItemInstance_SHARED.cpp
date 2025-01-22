// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Item/BuzzzItemInstance_SHARED.h"

#include "Container/BuzzzSubsystem.h"
#include "Item/BuzzzItemDefinition.h"

TMap<const UBuzzzItemDefinition*, UBuzzzItemInstance*> UBuzzzItemInstance_SHARED::InstanceMap = {};

UBuzzzItemInstance_SHARED::UBuzzzItemInstance_SHARED()
{
}

UBuzzzItemInstance* UBuzzzItemInstance_SHARED::MakeInstance_Implementation(
    const UBuzzzItemDefinition* InDefinition,
    AActor* Instigator) const
{
    const auto ExistInstancePtr = InstanceMap.Find(InDefinition);

    if (ExistInstancePtr)
    {
        return *ExistInstancePtr;
    }

    const auto Subsystem = Instigator->GetWorld()->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();

    const auto FreshInstance = NewObject<UBuzzzItemInstance_SHARED>(Subsystem, InDefinition->InstanceClass);
    InstanceMap.Add(InDefinition, FreshInstance);
    return FreshInstance;
}
