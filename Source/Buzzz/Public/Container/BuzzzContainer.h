// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuzzzFastArray.h"
#include "BuzzzContainer.generated.h"

class UBuzzzItemDefinition;
class UBuzzzItemInstance;
class UBuzzzContainer;

UENUM(BlueprintType)
enum class EBuzzzHiveMutationType : uint8
{
    None UMETA(Hidden),
    Remove,
    Add,
    Change,
};

USTRUCT(BlueprintType)
struct BUZZZ_API FBuzzzCellOperationContext
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

    static FBuzzzCellOperationContext& GetEmptyContext()
    {
        thread_local FBuzzzCellOperationContext EmptyContext{};
        return EmptyContext;
    }

    void Reset()
    {
        *this = GetEmptyContext();
    }
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuzzzCellMutationDelegate, const FBuzzzCellOperationContext&,
                                            Context);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBuzzzHiveMutationDelegate, const TArray<int32>&, Indices,
                                             const EBuzzzHiveMutationType, Type);

UCLASS(Blueprintable, Abstract, ClassGroup=(Buzzz), meta=(BlueprintSpawnableComponent))
class BUZZZ_API UBuzzzContainer : public UActorComponent
{
    GENERATED_BODY()

protected:
    UPROPERTY(
        // Instead of using RepNotify, Use FastArray Callback Functions
        // ReplicatedUsing="OnRep_Hive",
        Replicated,
        BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Buzzz")
    FBuzzzContainerHive Hive;

    // UFUNCTION(BlueprintNativeEvent, Category="Buzzz")
    // void OnRep_Hive();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Buzzz")
    int32 InitialCapacity = 0;

public:
    UBuzzzContainer();
    virtual void InitializeComponent() override;
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /**
     * We Should Bind Delegates Inside This
     */
    UFUNCTION(BlueprintNativeEvent, Category="Buzzz")
    void OnInitialization();

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

#pragma region Client
    /**
     * Client/Standalone Hive Change Callbacks (Not Triggered on Server)
     *
     * Client: Triggered when the client receives replication data via FastArray callbacks; mutations are batched.
     * Standalone: Triggered during each cell assignment; mutations are not batched.
     */
    UPROPERTY(BlueprintAssignable, Category="Buzzz | Client")
    FBuzzzHiveMutationDelegate Client_ReceiveHiveMutation;

    UFUNCTION()
    void HandleStandalonePostCellChanged(const FBuzzzCellOperationContext& Context);

    UFUNCTION()
    void HandleStandaloneHiveResize(const TArray<int32>& Indices, EBuzzzHiveMutationType ResizeType);
#pragma endregion

protected:
#pragma region Internal Authority Callbacks (Should Not Bind In Other Classes)

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz | Authority")
    FBuzzzCellMutationDelegate PreCellChange;
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz | Authority")
    FBuzzzCellMutationDelegate PostCellChange;
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz | Authority")
    FBuzzzCellMutationDelegate OnCellChange;
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz | Authority")
    FBuzzzHiveMutationDelegate OnHiveResize;
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz | Authority")
    FBuzzzCellMutationDelegate OnAssignFailed;

#pragma endregion

#pragma region Capacity
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", meta = (AutoCreateRefTerm = "NewCapacity"))
    bool Resize(const int32& NewCapacity);
#pragma endregion

#pragma region Assign Operation
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz")
    FBuzzzCellOperationContext AssignCell(UPARAM(ref) FBuzzzCellOperationContext& Context);
#pragma endregion


#pragma region Wrapper Operations
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz",
        meta = (AutoCreateRefTerm = "Index", ReturnDisplayName="Success"))
    bool ClearCell(const int32& Index, FBuzzzCellOperationContext& OutContext);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz",
        meta = (AutoCreateRefTerm = "Index,FromIndex", ReturnDisplayName="Success"))
    bool MergeCells(const int32& Index, UBuzzzContainer* FromContainer, const int32& FromIndex);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz",
        meta = (AutoCreateRefTerm = "Index,FromIndex", ReturnDisplayName="Success"))
    bool SwapCells(const int32& Index, UBuzzzContainer* FromContainer, const int32& FromIndex);
#pragma endregion
};
