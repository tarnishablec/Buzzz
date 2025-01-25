// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Net/Core/PushModel/PushModelMacros.h"
#include "UObject/Object.h"
#include "BuzzzFragment.generated.h"

class UBuzzzItem;
/**
 * 
 */
UCLASS(Blueprintable, Abstract, DefaultToInstanced, EditInlineNew, HideCategories="Runtime")

class BUZZZ_API UBuzzzFragment : public UObject
{
    GENERATED_BODY()

    REPLICATED_BASE_CLASS()

public:
    virtual bool IsSupportedForNetworking() const override { return true; };
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual UWorld* GetWorld() const override;
    virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
    virtual bool CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams,
                                    FFrame* Stack) override;

#if UE_WITH_IRIS
    virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                              UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    virtual void InitializeFragment();

    UFUNCTION(BlueprintNativeEvent)
    void OnInitialized(UBuzzzItem* Item);

    UFUNCTION(BlueprintNativeEvent)
    void OnDestroy();

    UFUNCTION(BlueprintPure)
    UBuzzzItem* GetHostItem() const;
};
