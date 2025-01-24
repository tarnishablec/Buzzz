// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Presets/BuzzzInstancingMode_PerItem.h"

#include "Buzzz/Core/Item/BuzzzInstance.h"

UBuzzzInstance* UBuzzzInstancingMode_PerItem::MakeInstance_Implementation(const UBuzzzDefinition* Definition,
                                                                          const TSubclassOf<UBuzzzInstance>
                                                                          InstanceClass,
                                                                          AActor* Creator) const
{
    const auto FreshInstance = NewObject<UBuzzzInstance>(Creator, InstanceClass);
    FreshInstance->Definition = Definition;
    return FreshInstance;
}
