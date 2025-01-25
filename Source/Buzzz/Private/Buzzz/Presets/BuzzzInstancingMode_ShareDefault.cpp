// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Presets/BuzzzInstancingMode_ShareDefault.h"

#include "Buzzz/Core/Item/BuzzzItem.h"

UBuzzzItem* UBuzzzInstancingMode_ShareDefault::MakeInstance_Implementation(
    const TSubclassOf<UBuzzzItem> InstanceClass, AActor* Creator) const
{
    return Cast<UBuzzzItem>(this->GetClass()->GetDefaultObject());
}
