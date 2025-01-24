// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Buzzz/Core/Item/BuzzzDefinition.h"

const FPrimaryAssetType UBuzzzDefinition::DataAssetType = BUZZZ_ITEM_DEFINITION_ASSET_NAME;


FPrimaryAssetId UBuzzzDefinition::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(DataAssetType, GetFName());
}

