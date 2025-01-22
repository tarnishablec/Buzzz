// Copyright 2019-Present tarnishablec. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "BuzzzItemInstance.h"
#include "BuzzzItemInstance_UNIQUE.h"
// #include "Container/BuzzzContainer.h"
#include "BuzzzItemInstance_LINK.generated.h"

class UBuzzzAction_WaitForInstanceDisconnect;
/**
 * 
 */
UCLASS(Blueprintable)
class BUZZZ_API UBuzzzItemInstance_LINK : public UBuzzzItemInstance_UNIQUE
{
    GENERATED_BODY()

public:
    UPROPERTY(Category="Buzzz", Replicated, Setter, BlueprintReadWrite)
    TObjectPtr<UBuzzzItemInstance> SourceInstance;

    void SetSourceInstance(UBuzzzItemInstance* InSourceInstance)
    {
        IsSourceValid = CheckSourceValid();
        SourceInstance = InSourceInstance;
    };

    UPROPERTY(BlueprintReadOnly, Category="Buzzz", Replicated)
    bool IsSourceValid = false;

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, BlueprintPure, Category="Buzzz")
    bool CheckSourceValid() const;

    virtual UBuzzzItemInstance* MakeInstance_Implementation(const UBuzzzItemDefinition* InDefinition,
                                                            AActor* Instigator) const override;

    virtual const UBuzzzFragment* FindFragmentByClass_Implementation(TSubclassOf<UBuzzzFragment> FragmentClass, bool Exact = true) const override;

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void HandleSourceDisconnect(UBuzzzItemInstance* ItemInstance, const UBuzzzContainer* Container);

    UFUNCTION(Client, Reliable)
    void Client_ReceiveSourceDisconnect(UBuzzzItemInstance* InSourceInstance, const UBuzzzContainer* Container);

    virtual void OnInitialization_Implementation() override;

    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForInstanceDisconnect> WaitSourceDisconnectAction;
};
