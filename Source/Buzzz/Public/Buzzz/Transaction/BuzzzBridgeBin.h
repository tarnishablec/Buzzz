// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "BuzzzBridgeBin.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BUZZZ_API UBuzzzBridgeBin : public UBuzzzContainer
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UBuzzzBridgeBin();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;
};
