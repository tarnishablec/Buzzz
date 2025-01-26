// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuzzzInstancingMode.h"
#include "UObject/Object.h"
#include "BuzzzItem.generated.h"

class UBuzzzInstancingMode;
class UBuzzzFragment;
class UBuzzzDefinition;
/**
 * 
 */
UCLASS(Blueprintable, Abstract, meta=(DontUseGenericSpawnObject="true"))
class BUZZZ_API UBuzzzItem : public UObject
{
    GENERATED_BODY()

public:
    friend class UBuzzzDefinition;
    UBuzzzItem();

#pragma region Overrides
    virtual bool IsSupportedForNetworking() const override { return true; };
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
    virtual bool CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams,
                                    FFrame* Stack) override;
    virtual void BeginDestroy() override;
    virtual class UWorld* GetWorld() const override;

#if UE_WITH_IRIS
    virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                              UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif
    virtual void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& ObjList) override;
#pragma endregion

    UPROPERTY(Replicated, BlueprintReadOnly, SaveGame)
    FGuid ItemGuid = FGuid::NewGuid();

    UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly, SaveGame,
        meta=(AllowAbstract=false))
    TSubclassOf<UBuzzzInstancingMode> InstancingMode = UBuzzzInstancingMode::StaticClass();

protected:
    UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true), SaveGame)
    TArray<TObjectPtr<UBuzzzFragment>> Fragments;

public:
#pragma region Helpers
    UFUNCTION(BlueprintPure, Category = "Buzzz")
    FGuid GetItemGuid() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Buzzz",
        meta = (DeterminesOutputType = "FragmentClass"))
    const UBuzzzFragment* FindFragmentByClass(
        UPARAM(meta=(AllowAbstract=true)) TSubclassOf<UBuzzzFragment> FragmentClass, bool Exact = true) const;

    template <class T, typename = typename TEnableIf<TPointerIsConvertibleFromTo<
                  T, UBuzzzFragment>::Value>::Type>
    T* FindFragmentByClass(bool Exact = true) const
    {
        return Cast<T>(FindFragmentByClass(T::StaticClass(), Exact));
    }

    UFUNCTION(BlueprintPure, Category = "Buzzz", DisplayName="Make Instance",
        meta=(DeterminesOutputType="ItemClass"))
    static UBuzzzItem* MakeInstance_Static(
        TSubclassOf<UBuzzzItem> ItemClass,
        AActor* Creator);

#pragma endregion Helpers

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly)
    void OnInitialization();

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly)
    void PostInitialized();

    UFUNCTION(BlueprintNativeEvent)
    void InitializeFragments();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly)
    void Initialize();

protected:
    bool bInitialized = false;
};
