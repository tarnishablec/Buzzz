// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "BuzzzItem_UNIQUE.h"
#include "BuzzzItem_LINK.generated.h"

class UBuzzzAction_WaitForItemRemovalOrAddition;
/**
 * 
 */
UCLASS(Blueprintable)
class BUZZZ_API UBuzzzItem_LINK : public UBuzzzItem_UNIQUE
{
    GENERATED_BODY()

public:
    UPROPERTY(Category="Buzzz", Replicated, Setter, BlueprintReadWrite, SaveGame, meta=(ExposeOnSpawn))
    TObjectPtr<UBuzzzItem> SourceInstance;

    void SetSourceInstance(UBuzzzItem* InSourceInstance)
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
    void HandleSourceDisconnect(UBuzzzItem* Item, const UBuzzzContainer* Container);

    UFUNCTION(Client, Reliable)
    void Client_ReceiveSourceDisconnect(UBuzzzItem* InSourceInstance, const UBuzzzContainer* Container);

    virtual void OnInitialization_Implementation() override;

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForItemRemovalOrAddition> WaitSourceDisconnectAction;
};
