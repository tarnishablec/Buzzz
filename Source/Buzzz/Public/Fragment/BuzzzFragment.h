// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BuzzzFragment.generated.h"

class UBuzzzItemInstance;
/**
 * 
 */
UCLASS(Blueprintable, Abstract, DefaultToInstanced, EditInlineNew, HideCategories="Runtime")

class BUZZZ_API UBuzzzFragment : public UObject
{
    GENERATED_BODY()

public:
    virtual bool IsSupportedForNetworking() const override { return true; };
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual UWorld* GetWorld() const override;
    virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
    virtual bool CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams,
                                    FFrame* Stack) override;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    virtual void InitializeFragment(UBuzzzItemInstance* HostItemInstance);

    UFUNCTION(BlueprintNativeEvent)
    void OnInitialized(UBuzzzItemInstance* ItemInstance);

    UFUNCTION(BlueprintPure)
    UBuzzzItemInstance* GetHostItemInstance() const;
};
