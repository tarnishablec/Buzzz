// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuzzzFastArray.h"
#include "BuzzzContainer.generated.h"

class UBuzzzItemDefinition;
class UBuzzzItemInstance;
class UBuzzzContainer;

USTRUCT(BlueprintType)
struct BUZZZ_API FBuzzzOperationContext
{
    GENERATED_BODY()

    // Input

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBuzzzContainer> TargetContainer;

    UPROPERTY(BlueprintReadWrite)
    int32 TargetIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBuzzzItemInstance> UpcomingInstance;

    UPROPERTY(BlueprintReadWrite)
    int32 UpcomingStackCount = -1;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBuzzzContainer> FromContainer;

    UPROPERTY(BlueprintReadWrite)
    int32 FromIndex = INDEX_NONE;

    // Output

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBuzzzItemInstance> PreviousInstance;

    UPROPERTY(BlueprintReadOnly)
    int32 PreviousStackCount = -1;

    UPROPERTY(BlueprintReadOnly)
    bool bSuccess = false;

    UPROPERTY(BlueprintReadOnly)
    bool bFinished = false;

    static FBuzzzOperationContext& GetEmptyContext()
    {
        thread_local FBuzzzOperationContext EmptyContext{};
        return EmptyContext;
    }

    void Reset()
    {
        *this = GetEmptyContext();
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuzzzContainerOperationDelegate, const FBuzzzOperationContext&, Context);

UCLASS(Blueprintable, Abstract, ClassGroup=(Buzzz), meta=(BlueprintSpawnableComponent))
class BUZZZ_API UBuzzzContainer : public UActorComponent
{
    GENERATED_BODY()

protected:
    UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    FBuzzzContainerHive Hive;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    int32 Capacity = 0;

public:
    virtual void InitializeComponent() override;
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    FCriticalSection ContainerCS;

public:
    UBuzzzContainer();

#pragma region Helpers
    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Buzzz")
    bool CheckItemCompatible(const UBuzzzItemInstance* ItemInstance) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    bool CheckItemInstanceOwned(const UBuzzzItemInstance* ItemInstance) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index"))
    bool CheckCellEmpty(const int32& Index) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index"))
    const FBuzzzContainerCell& GetCell(const int32& Index, bool& IsValidIndex) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index"))
    bool CheckCellHasItem(const int32& Index, const UBuzzzItemDefinition* ItemDefinition) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta=(ReturnDisplayName="Amount"))
    int32 CalcTotalAmountOfItem(const UBuzzzItemDefinition* ItemDefinition);

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta=(ReturnDisplayName="EmptyIndex"))
    int32 FindEmptySlot(bool& Found) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index"))
    bool CheckIndexIsValid(const int32& Index) const;
#pragma endregion Helpers

protected:
#pragma region Delegates
    UPROPERTY(BlueprintAssignable)
    FBuzzzContainerOperationDelegate PreCellChange;
    UPROPERTY(BlueprintAssignable)
    FBuzzzContainerOperationDelegate PostCellChange;
    UPROPERTY(BlueprintAssignable)
    FBuzzzContainerOperationDelegate OnCellChange;
    UPROPERTY(BlueprintAssignable)
    FBuzzzContainerOperationDelegate OnAssignFailed;
#pragma endregion Delegates

#pragma region Callbacks

#pragma endregion Callbacks

public:
#pragma region Assign
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz")
    FBuzzzOperationContext AssignCell(const FBuzzzOperationContext& Context);
#pragma endregion Assign

#pragma region Wrapper Operations
    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "Buzzz",
        meta = (AutoCreateRefTerm = "Index", ReturnDisplayName="Success"))
    bool ClearCell(const int32& Index, FBuzzzOperationContext& OutContext);

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "Buzzz",
        meta = (AutoCreateRefTerm = "Index,FromIndex", ReturnDisplayName="Success"))
    bool MergeCells(const int32& Index, UBuzzzContainer* FromContainer, const int32& FromIndex);

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "Buzzz",
        meta = (AutoCreateRefTerm = "Index,FromIndex", ReturnDisplayName="Success"))
    bool SwitchCells(const int32& Index, UBuzzzContainer* FromContainer, const int32& FromIndex);
#pragma endregion Wrapper Operations
};
