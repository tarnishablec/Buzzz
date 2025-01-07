// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Item/BuzzzItemInstance_SHARED.h"
#include "Item/BuzzzItemDefinition.h"

UBuzzzItemInstance_SHARED::UBuzzzItemInstance_SHARED()
{
}

void UBuzzzItemInstance_SHARED::InitializeInstance(const UBuzzzItemDefinition* ItemDefinition)
{
    if (bInitialized)
    {
        checkNoEntry();
    }

    InitializeFragments();
    bInitialized = true;
}
