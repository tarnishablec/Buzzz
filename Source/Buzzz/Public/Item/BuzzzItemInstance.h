// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BuzzzItemInstance.generated.h"

class UBuzzzFragment;
class UBuzzzItemDefinition;
class UBuzzzContainer;
/**
 * 
 */
UCLASS(Blueprintable)
class BUZZZ_API UBuzzzItemInstance : public UObject
{
    GENERATED_BODY()

public:
    friend class UBuzzzContainer;

    UBuzzzItemInstance();
    virtual bool IsSupportedForNetworking() const override { return true; };
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
    virtual bool CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams,
                                    FFrame* Stack) override;

protected:
    UPROPERTY(Replicated)
    TObjectPtr<const UBuzzzItemDefinition> Definition;

    UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    TObjectPtr<const UObject> Creator;

    UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    FGuid ItemGuid;

    UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    TArray<TObjectPtr<UBuzzzFragment>> Fragments;

public:
    UPROPERTY(EditDefaultsOnly, Category = "Buzzz Item Instance")
    bool ShouldReplicate = true;

    UFUNCTION(BlueprintPure, Category = "Buzzz | Item | Instance")
    UBuzzzContainer* GetOwningContainer() const;

    UFUNCTION(BlueprintPure, Category = "Buzzz | Item | Instance")
    const UObject* GetCreator() const;

    UFUNCTION(BlueprintPure, Category = "Buzzz | Item | Instance")
    FGuid GetItemGuid() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Buzzz | Item | Instance")
    const UBuzzzItemDefinition* GetDefinition() const;

    UFUNCTION(BlueprintCallable, Category = "Buzzz | Item | Instance")
    void Destroy();

    UFUNCTION()
    void InitializeItemInstance(const UBuzzzItemDefinition* ItemDefinition);

    UFUNCTION(BlueprintNativeEvent, Category = "Buzzz | Item | Instance")
    void OnInitialized();

    UFUNCTION()
    void InitializeFragments();

    UFUNCTION(BlueprintPure, Category = "Buzzz | Item | Instance", meta = (DeterminesOutputType = "FragmentClass"))
    const UBuzzzFragment* FindFragmentByClass(
        UPARAM(meta=(AllowAbstract=true)) const TSubclassOf<UBuzzzFragment>& FragmentClass, bool Exact = true) const;

    template <class T, typename = typename TEnableIf<TPointerIsConvertibleFromTo<
                  T, UBuzzzFragment>::Value>::Type>
    T* FindFragmentByClass(bool Exact) const
    {
        // ReSharper disable once CppCStyleCast
        return (T*)(FindFragmentByClass(T::StaticClass(), Exact));
    }
};
