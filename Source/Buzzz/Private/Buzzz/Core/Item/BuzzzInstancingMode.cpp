// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Core/Item/BuzzzInstancingMode.h"
#include "Buzzz/Core/Item/BuzzzItem.h"

UBuzzzItem* UBuzzzInstancingMode::MakeInstance_Implementation(
    TSubclassOf<UBuzzzItem> InstanceClass,
    AActor* Creator) const
{
    return nullptr;
}
