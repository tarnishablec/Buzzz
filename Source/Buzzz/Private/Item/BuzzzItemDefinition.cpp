// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Item/BuzzzItemDefinition.h"
#include "Item/BuzzzItemInstance.h"

const FPrimaryAssetType UBuzzzItemDefinition::DataAssetType = BUZZZ_ITEM_DEFINITION_ASSET_NAME;


FPrimaryAssetId UBuzzzItemDefinition::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(DataAssetType, GetFName());
}

UBuzzzItemInstance* UBuzzzItemDefinition::Instantiate(AActor* Instigator) const
{
    check(Instigator);

    const auto Instance = UBuzzzItemInstance::MakeInstanceFromDefinition(
        this, Instigator);

    Instance->Definition = this;
    Instance->Initialize();
    return Instance;
}
