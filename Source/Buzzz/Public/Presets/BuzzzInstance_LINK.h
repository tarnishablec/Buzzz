// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "Core/Item/BuzzzInstance.h"
#include "BuzzzInstance_UNIQUE.h"
#include "BuzzzInstance_LINK.generated.h"

class UBuzzzAction_WaitForInstanceDisconnect;
/**
 * 
 */
UCLASS(Blueprintable)
class BUZZZ_API UBuzzzInstance_LINK : public UBuzzzInstance_UNIQUE
{
    GENERATED_BODY()

public:
    UBuzzzInstance_LINK()
    {
    }

    UPROPERTY(Category="Buzzz", Replicated, Setter, BlueprintReadWrite, SaveGame)
    TObjectPtr<UBuzzzInstance> SourceInstance;

    void SetSourceInstance(UBuzzzInstance* InSourceInstance)
    {
        IsSourceValid = CheckSourceValid();
        SourceInstance = InSourceInstance;
    };

    UPROPERTY(BlueprintReadOnly, Category="Buzzz", Replicated)
    bool IsSourceValid = false;

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, BlueprintPure, Category="Buzzz")
    bool CheckSourceValid() const;

    virtual const UBuzzzFragment* FindFragmentByClass_Implementation(TSubclassOf<UBuzzzFragment> FragmentClass,
                                                                     bool Exact = true) const override;

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void HandleSourceDisconnect(UBuzzzInstance* ItemInstance, const UBuzzzContainer* Container);

    UFUNCTION(Client, Reliable)
    void Client_ReceiveSourceDisconnect(UBuzzzInstance* InSourceInstance, const UBuzzzContainer* Container);

    virtual void OnInitialization_Implementation() override;

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForInstanceDisconnect> WaitSourceDisconnectAction;
};
