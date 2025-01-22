// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/BuzzzItemDefinition.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BuzzzLibrary.generated.h"

class UBuzzzItemDefinition;
struct FBuzzzContainerCell;
class UBuzzzItemInstance;
class UBuzzzContainer;
/**
 * 
 */
UCLASS()
class BUZZZ_API UBuzzzLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    static void CellGetContainerInfo(const FBuzzzContainerCell& Cell, UBuzzzContainer*& Container, int32& Index);
};
