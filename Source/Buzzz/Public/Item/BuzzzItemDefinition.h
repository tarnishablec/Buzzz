// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuzzzItemDefinition.generated.h"

class UBuzzzContainer;
class UBuzzzItemInstance;
class UBuzzzFragment;
/**
 * 
 */

#define BUZZZ_ITEM_DEFINITION_ASSET_NAME TEXT("BuzzzItemDefinition")

UCLASS(Blueprintable, Abstract)
class BUZZZ_API UBuzzzItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    static const FPrimaryAssetType DataAssetType;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Instance", NoClear)
    TSubclassOf<UBuzzzItemInstance> InstanceClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragments", Instanced, NoClear,
        meta = (NoElementDuplicate))
    TArray<TObjectPtr<UBuzzzFragment>> FragmentsTemplate;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly)
    UBuzzzItemInstance* CreateItemInstance(UObject* Instigator);
};
