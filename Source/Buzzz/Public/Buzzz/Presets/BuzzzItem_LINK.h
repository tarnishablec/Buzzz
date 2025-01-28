// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "BuzzzItem_UNIQUE.h"
#include "BuzzzItem_LINK.generated.h"

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
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

    UPROPERTY(BlueprintReadOnly, Category="Buzzz", ReplicatedUsing=OnRep_IsSourceValid)
    bool IsSourceValid = false;

    UFUNCTION()
    void OnRep_IsSourceValid();
    
    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, BlueprintPure, Category="Buzzz")
    bool CheckSourceValid() const;

    virtual const UBuzzzFragment* FindFragmentByClass_Implementation(TSubclassOf<UBuzzzFragment> FragmentClass,
                                                                     bool Exact = true) const override;

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void OnInitialization_Implementation() override;

    virtual void PreDemolish_Implementation() override;

protected:
    UPROPERTY()
    TObjectPtr<UBuzzzAction_WaitForItemRemovalOrAddition> WaitForSourceRemovalAction;

    UFUNCTION()
    void HandleSourceRemoval(const FBuzzzItemTransferContext& Context);
};
