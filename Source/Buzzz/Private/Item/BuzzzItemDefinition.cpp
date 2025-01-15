// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Item/BuzzzItemDefinition.h"
#include "Item/BuzzzItemInstance.h"

const FPrimaryAssetType UBuzzzItemDefinition::DataAssetType = BUZZZ_ITEM_DEFINITION_ASSET_NAME;

const UBuzzzItemInstance* UBuzzzItemDefinition::Instantiate_Implementation(AActor* Instigator) const
{
    check(IsValid(InstanceClass))
    const auto Instance = InstanceClass.GetDefaultObject()->MakeInstance(this, Instigator);
    return Instance;
}

FPrimaryAssetId UBuzzzItemDefinition::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(DataAssetType, GetFName());
}
