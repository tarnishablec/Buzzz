// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuzzzItemDefinition.generated.h"

class UBuzzzContainer;
class UBuzzzItemInstance;
class UBuzzzFragment;

#define BUZZZ_ITEM_DEFINITION_ASSET_NAME TEXT("BuzzzItemDefinition")

UCLASS(Blueprintable, Abstract, Const)
class BUZZZ_API UBuzzzItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    static const FPrimaryAssetType DataAssetType;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Instance", NoClear, meta=(AllowAbstract=false))
    TSubclassOf<UBuzzzItemInstance> InstanceClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragments", Instanced, NoClear,
        meta = (NoElementDuplicate))
    TArray<TObjectPtr<UBuzzzFragment>> FragmentsTemplate;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz")
    UBuzzzItemInstance* Instantiate(AActor* Instigator) const;
};
