// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuzzzItemDefinition.h"
#include "UObject/Object.h"
#include "BuzzzItemInstance.generated.h"

class UBuzzzFragment;
class UBuzzzItemDefinition;
class UBuzzzContainer;
/**
 * 
 */
UCLASS(BlueprintType, Abstract, NotBlueprintable)
class BUZZZ_API UBuzzzItemInstance : public UObject
{
    GENERATED_BODY()

public:
    friend class UBuzzzContainer;
    friend class UBuzzzItemDefinition;

    UBuzzzItemInstance();
    virtual bool IsSupportedForNetworking() const override { return true; };
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
    virtual bool CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams,
                                    FFrame* Stack) override;

    virtual void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& ObjList) override;
    virtual auto BeginDestroy() -> void override;

protected:
    UPROPERTY(Replicated)
    TObjectPtr<const UBuzzzItemDefinition> Definition;

    UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    FGuid ItemGuid;

    UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    TArray<TObjectPtr<UBuzzzFragment>> Fragments;

public:
    UPROPERTY(EditDefaultsOnly, Category = "Buzzz")
    bool ShouldReplicate = true;

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    UBuzzzContainer* GetOwningContainer() const;

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    FGuid GetItemGuid() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Buzzz")
    const UBuzzzItemDefinition* GetDefinition() const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (DeterminesOutputType = "FragmentClass"))
    const UBuzzzFragment* FindFragmentByClass(
        UPARAM(meta=(AllowAbstract=true)) const TSubclassOf<UBuzzzFragment>& FragmentClass, bool Exact = true) const;

    template <class T, typename = typename TEnableIf<TPointerIsConvertibleFromTo<
                  T, UBuzzzFragment>::Value>::Type>
    T* FindFragmentByClass(bool Exact) const
    {
        // ReSharper disable once CppCStyleCast
        return (T*)(FindFragmentByClass(T::StaticClass(), Exact));
    }

    UFUNCTION(BlueprintNativeEvent, meta=(ForceAsFunction))
    UBuzzzItemInstance* OnInstantiation(const UBuzzzItemDefinition* ItemDefinition);

    UFUNCTION(BlueprintNativeEvent)
    UBuzzzItemInstance* MakeInstance(const UBuzzzItemDefinition* InDefinition) const;

protected:
    bool bInitialized = false;
    virtual void InitializeFragments();

    virtual void InitializeInstance();
};
