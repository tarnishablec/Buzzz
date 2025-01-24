// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Core/Item/BuzzzInstancingMode.h"
#include "Buzzz/Core/Item/BuzzzDefinition.h"
#include "Buzzz/Core/Item/BuzzzInstance.h"

UBuzzzInstance* UBuzzzInstancingMode::MakeInstance_Implementation(const UBuzzzDefinition* Definition,
                                                                  TSubclassOf<UBuzzzInstance> InstanceClass,
                                                                  AActor* Creator) const
{
    return nullptr;
}

// ReSharper disable once CppPassValueParameterByConstReference
UBuzzzInstance* UBuzzzInstancingMode::MakeInstance_Static(const UBuzzzDefinition* Definition,
                                                          const TSubclassOf<UBuzzzInstance> InstanceClass,
                                                          AActor* Creator)
{
    const auto FinalClass =
        IsValid(InstanceClass) ? InstanceClass : Definition ? Definition->InstanceClass : nullptr;

    check(FinalClass);

    if (!IsValid(FinalClass))
    {
        return nullptr;
    }

    const auto FinalInstance = FinalClass.GetDefaultObject()->InstancingMode.GetDefaultObject()->MakeInstance(
        Definition, FinalClass, Creator);
    
    return FinalInstance;
}
