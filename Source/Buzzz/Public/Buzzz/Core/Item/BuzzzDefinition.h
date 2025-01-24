// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuzzzDefinition.generated.h"

class UBuzzzContainer;
class UBuzzzInstance;
class UBuzzzFragment;

#define BUZZZ_ITEM_DEFINITION_ASSET_NAME TEXT("BuzzzItemDefinition")

UCLASS(Blueprintable, Abstract, Const)
class BUZZZ_API UBuzzzDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(SaveGame, EditDefaultsOnly)
    FGuid DefinitionGuid = FGuid::NewGuid();

    static const FPrimaryAssetType DataAssetType;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Instance", NoClear, meta=(AllowAbstract=false))
    TSubclassOf<UBuzzzInstance> InstanceClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragments", Instanced, NoClear,
        meta = (NoElementDuplicate))
    TArray<TObjectPtr<UBuzzzFragment>> FragmentsTemplate;
};
