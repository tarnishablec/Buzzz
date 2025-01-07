// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Item/BuzzzItemInstance_UNIQUE.h"
#include "Item/BuzzzItemDefinition.h"


void UBuzzzItemInstance_UNIQUE::InitializeInstance(const UBuzzzItemDefinition* ItemDefinition)
{
    Definition = ItemDefinition;
    check(IsValid(Definition));

    InitializeFragments();
    OnInitialized();
    bInitialized = true;
}

void UBuzzzItemInstance_UNIQUE::OnInitialized_Implementation()
{
    check(IsValid(Definition));
}

