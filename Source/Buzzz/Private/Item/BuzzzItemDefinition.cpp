// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/BuzzzItemDefinition.h"
#include "Item/BuzzzItemInstance.h"

const FPrimaryAssetType UBuzzzItemDefinition::DataAssetType = BUZZZ_ITEM_DEFINITION_ASSET_NAME;

UBuzzzItemInstance* UBuzzzItemDefinition::CreateItemInstance_Implementation(UObject* Instigator)
{
    if (!IsValid(Instigator))
    {
        checkNoEntry();
        return nullptr;
    }

    const auto Instance = NewObject<UBuzzzItemInstance>(Instigator, InstanceClass);
    Instance->InitializeItemInstance(this);
    return Instance;
}

FPrimaryAssetId UBuzzzItemDefinition::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(DataAssetType, GetFName());
}
