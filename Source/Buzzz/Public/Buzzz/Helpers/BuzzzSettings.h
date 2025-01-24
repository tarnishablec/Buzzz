// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Buzzz/Transaction/BuzzzTransactionBridge.h"
#include "BuzzzSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Buzzz"))
class BUZZZ_API UBuzzzSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    virtual FName GetCategoryName() const override
    {
        return FName("Plugins");
    };


    UPROPERTY(GlobalConfig, EditAnywhere, Category="Transaction", NoClear)
    TSubclassOf<ABuzzzTransactionBridge> TransactionBridgeClass = ABuzzzTransactionBridge::StaticClass();

    static UBuzzzSettings* Get()
    {
        const auto Setting = GetMutableDefault<UBuzzzSettings>();
        return Setting;
    };
};
