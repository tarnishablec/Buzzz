// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuzzzItemDefinition.h"
#include "UObject/Object.h"
#include "BuzzzItemInstance.generated.h"

class UBuzzzFragment;
class UBuzzzItemDefinition;
/**
 * 
 */
UCLASS(BlueprintType, Abstract)
class BUZZZ_API UBuzzzItemInstance : public UObject
{
    GENERATED_BODY()

public:
    friend class UBuzzzItemDefinition;

    UBuzzzItemInstance();
    virtual bool IsSupportedForNetworking() const override { return true; };
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
    virtual bool CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams,
                                    FFrame* Stack) override;

#if UE_WITH_IRIS
    virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                              UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif

    virtual void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& ObjList) override;

protected:
    UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    TObjectPtr<const UBuzzzItemDefinition> Definition;

    UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    FGuid ItemGuid;

    UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    TArray<TObjectPtr<UBuzzzFragment>> Fragments;

public:
#pragma region Helpers
    UFUNCTION(BlueprintPure, Category = "Buzzz")
    FGuid GetItemGuid() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Buzzz")
    const UBuzzzItemDefinition* GetDefinition() const;

    UFUNCTION(BlueprintPure, Category = "Buzzz",
        meta = (DeterminesOutputType = "FragmentClass"))
    const UBuzzzFragment* FindFragmentByClass(
        UPARAM(meta=(AllowAbstract=true)) TSubclassOf<UBuzzzFragment> FragmentClass, bool Exact = true) const;

    template <class T, typename = typename TEnableIf<TPointerIsConvertibleFromTo<
                  T, UBuzzzFragment>::Value>::Type>
    T* FindFragmentByClass(bool Exact = true) const
    {
        return Cast<T>(FindFragmentByClass(T::StaticClass(), Exact));
    }

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Buzzz")
    AActor* GetOwnerActor() const;
#pragma endregion Helpers

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly)
    void OnInitialization();

    UFUNCTION(BlueprintNativeEvent)
    UBuzzzItemInstance* MakeInstance(const UBuzzzItemDefinition* InDefinition, AActor* Instigator) const;

protected:
    bool bInitialized = false;
    virtual void InitializeFragments();
    virtual void InitializeInstance();
};
