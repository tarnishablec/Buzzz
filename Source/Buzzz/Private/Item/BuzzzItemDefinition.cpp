// Copyright 2019-Present tarnishablec. All Rights Reserved.

#include "Item/BuzzzItemDefinition.h"
#include "Item/BuzzzItemInstance.h"
#include "Item/BuzzzItemInstance_SHARED.h"
#include "Item/BuzzzItemInstance_UNIQUE.h"

const FPrimaryAssetType UBuzzzItemDefinition::DataAssetType = BUZZZ_ITEM_DEFINITION_ASSET_NAME;

const UBuzzzItemInstance* UBuzzzItemDefinition::Instantiate_Implementation() const
{
    const UBuzzzItemInstance* FinalInstance = nullptr;

    if (InstanceClass->IsChildOf(UBuzzzItemInstance_SHARED::StaticClass()))
    {
        FinalInstance = InstanceClass.GetDefaultObject();
    }
    else if (InstanceClass->IsChildOf(UBuzzzItemInstance_UNIQUE::StaticClass()))
    {
        const auto UniqueInstance = NewObject<UBuzzzItemInstance_UNIQUE>(GetTransientPackage(), InstanceClass);
        UniqueInstance->InitializeInstance(this);
        FinalInstance = UniqueInstance;
    }
    else
    {
        checkNoEntry();
    }

    return FinalInstance;
}

FPrimaryAssetId UBuzzzItemDefinition::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(DataAssetType, GetFName());
}
