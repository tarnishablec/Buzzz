// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BuzzzLibrary.generated.h"

class UBuzzzItemInstance;
class UBuzzzContainer;
/**
 * 
 */
UCLASS()
class BUZZZ_API UBuzzzLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    //
    // UFUNCTION(BlueprintPure, Category = "Buzzz")
    // static UBuzzzContainer* GetContainerOwningInstance(const UBuzzzItemInstance* Instance);
    //
    // UFUNCTION(BlueprintPure, Category = "Buzzz")
    // static AActor* GetActorOwningInstance(const UBuzzzItemInstance* Instance);
};
