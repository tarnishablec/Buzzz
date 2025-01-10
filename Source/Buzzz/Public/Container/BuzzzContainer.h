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

UENUM(BlueprintType)
enum EBuzzzHiveReplicationType
{
    Remove,
    Add,
    Change,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuzzzContainerOperationDelegate, const FBuzzzOperationContext&, Context);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBuzzzReplicationCallback, const TArray<int32>&, Indices,
                                             EBuzzzHiveReplicationType, Type);

UCLASS(Blueprintable, Abstract, ClassGroup=(Buzzz), meta=(BlueprintSpawnableComponent))
class BUZZZ_API UBuzzzContainer : public UActorComponent
{
    GENERATED_BODY()

protected:
    UPROPERTY(
        // We Dont use RepNotify, Use FastArray Callback Functions
        // ReplicatedUsing="OnRep_Hive",
        Replicated,
        BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Buzzz")
    FBuzzzContainerHive Hive;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Buzzz")
    int32 InitialCapacity = 0;

public:
    UBuzzzContainer();
    virtual void InitializeComponent() override;
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintNativeEvent, Category="Buzzz")
    void OnInitialized();

private:
    FCriticalSection ContainerCS;

public:
#pragma region Helpers

    UFUNCTION(BlueprintPure, Category="Buzzz")
    int32 GetCapacity() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Buzzz")
    bool CheckItemCompatible(const UBuzzzItemInstance* ItemInstance) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    bool CheckItemInstanceOwned(const UBuzzzItemInstance* ItemInstance) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index"))
    bool CheckCellEmpty(const int32& Index) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index"))
    const FBuzzzContainerCell& GetCell(const int32& Index, bool& IsValidIndex) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index"))
    bool CheckCellHasItemByDefinition(const int32& Index, const UBuzzzItemDefinition* ItemDefinition) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index,DefinitionClass"))
    bool CheckCellHasItemByDefinitionClass(const int32& Index, const TSubclassOf<UBuzzzItemDefinition>& DefinitionClass,
                                           bool bStrict) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta=(ReturnDisplayName="Amount"))
    int32 CalcTotalAmountOfItem(const UBuzzzItemDefinition* ItemDefinition) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta=(ReturnDisplayName="EmptyIndex"))
    int32 FindEmptyCell(bool& Found) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index"))
    bool CheckIndexIsValid(const int32& Index) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    void FindIndexByInstance(const UBuzzzItemInstance* ItemInstance, TArray<int32>& OutIndexArray, int32& First,
                             int32& Last,
                             bool& Found) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    void FindIndexByDefinition(const UBuzzzItemDefinition* Definition, bool bStrict, TArray<int32>& OutIndexArray,
                               int32& First, int32& Last,
                               bool& Found) const;
#pragma endregion

#pragma region Client Replication Callbacks Through FastArray Hooks
    UPROPERTY(BlueprintAssignable, Category="Buzzz"
        // , meta=(a)
    )
    FBuzzzReplicationCallback ReceiveReplication;
#pragma endregion

protected:
#pragma region Authority Callbacks

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz")
    FBuzzzContainerOperationDelegate PreCellChange;
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz")
    FBuzzzContainerOperationDelegate PostCellChange;
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz")
    FBuzzzContainerOperationDelegate OnCellChange;
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz")
    FBuzzzContainerOperationDelegate OnAssignFailed;

#pragma endregion

#pragma region Assign
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz")
    FBuzzzOperationContext AssignCell(UPARAM(ref) FBuzzzOperationContext& Context);
#pragma endregion

#pragma region Capacity
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", meta = (AutoCreateRefTerm = "NewCapacity"))
    bool Resize(const int32& NewCapacity);
#pragma endregion

#pragma region Wrapper Operations
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz",
        meta = (AutoCreateRefTerm = "Index", ReturnDisplayName="Success"))
    bool ClearCell(const int32& Index, FBuzzzOperationContext& OutContext);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz",
        meta = (AutoCreateRefTerm = "Index,FromIndex", ReturnDisplayName="Success"))
    bool MergeCells(const int32& Index, UBuzzzContainer* FromContainer, const int32& FromIndex);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz",
        meta = (AutoCreateRefTerm = "Index,FromIndex", ReturnDisplayName="Success"))
    bool SwapCells(const int32& Index, UBuzzzContainer* FromContainer, const int32& FromIndex);
#pragma endregion
};
