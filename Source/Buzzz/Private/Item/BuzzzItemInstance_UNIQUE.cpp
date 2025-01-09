// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Item/BuzzzItemInstance_UNIQUE.h"

#include "Item/BuzzzItemDefinition.h"


void UBuzzzItemInstance_UNIQUE::OnInitialized_Implementation()
{
    check(IsValid(Definition));
}

UBuzzzItemInstance* UBuzzzItemInstance_UNIQUE::MakeInstance_Implementation(
    const UBuzzzItemDefinition* InDefinition, AActor* Instigator) const
{
    check(IsValid(InDefinition) && IsValid(Instigator));

    const auto Instance = NewObject<UBuzzzItemInstance_UNIQUE>(Instigator, InDefinition->InstanceClass);
    Instance->Definition = InDefinition;
    return Instance;
}
