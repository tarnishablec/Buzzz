// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Presets/BuzzzInstancingMode_PerItem.h"

#include "Core/Item/BuzzzInstance.h"

UBuzzzInstance* UBuzzzInstancingMode_PerItem::MakeInstance_Implementation(const UBuzzzDefinition* Definition,
                                                                          const TSubclassOf<UBuzzzInstance>
                                                                          InstanceClass,
                                                                          AActor* Creator) const
{
    const auto FreshInstance = NewObject<UBuzzzInstance>(Creator, InstanceClass);
    FreshInstance->Definition = Definition;
    return FreshInstance;
}
