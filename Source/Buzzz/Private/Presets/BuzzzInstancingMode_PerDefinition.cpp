// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Presets/BuzzzInstancingMode_PerDefinition.h"

#include "Core/Item/BuzzzDefinition.h"
#include "Core/Item/BuzzzInstance.h"
#include "Subsystem/BuzzzSubsystem.h"

UBuzzzInstance* UBuzzzInstancingMode_PerDefinition::MakeInstance_Implementation(const UBuzzzDefinition* Definition,
    const TSubclassOf<UBuzzzInstance> InstanceClass, AActor* Creator) const
{
    check(IsValid(Definition));
    check(Definition->DefinitionGuid.IsValid());

    const auto Subsystem = UBuzzzSubsystem::Get(Creator);

    const auto InstanceSet = Subsystem->GetInstancesByDefinition(Definition);

    if (InstanceSet.IsEmpty())
    {
        const auto FreshInstance = NewObject<UBuzzzInstance>(Creator, InstanceClass);
        FreshInstance->Definition = Definition;
        return FreshInstance;
    }

    return InstanceSet.Array().Last();
}
