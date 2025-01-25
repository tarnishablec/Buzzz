// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Presets/BuzzzInstancingMode_Unique.h"

#include "Buzzz/Core/Item/BuzzzItem.h"

UBuzzzItem* UBuzzzInstancingMode_Unique::MakeInstance_Implementation(
    const TSubclassOf<UBuzzzItem> InstanceClass,
    AActor* Creator) const
{
    const auto FreshInstance = NewObject<UBuzzzItem>(Creator, InstanceClass);
    return FreshInstance;
}
