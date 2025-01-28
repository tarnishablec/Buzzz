// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuzzzFastArray.h"
#include "NativeGameplayTags.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "BuzzzContainer.generated.h"

class UBuzzzDefinition;
class UBuzzzItem;
class UBuzzzContainer;
struct FBuzzzAssignmentContext;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_BuzzzEvent);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_BuzzzEvent_CellMutation);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_BuzzzEvent_HiveResize);

UCLASS(Blueprintable, Abstract, ClassGroup=(Buzzz), meta=(BlueprintSpawnableComponent))
class BUZZZ_API UBuzzzContainer : public UActorComponent
{
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Buzzz", meta=(AllowPrivateAccess=true))
    TEnumAsByte<ELifetimeCondition> HiveReplicateCondition = COND_OwnerOnly;

    UPROPERTY(
        // Instead of using RepNotify, Use FastArray Callback Functions
        // ReplicatedUsing="OnRep_Hive",
        Replicated,
        BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Buzzz")
    FBuzzzContainerHive Hive;

    // UFUNCTION(BlueprintNativeEvent, Category="Buzzz")
    // void OnRep_Hive();

    UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="Buzzz")
    FGuid ContainerGuid;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess, ClampMin=0), Category="Buzzz")
    int32 InitialCapacity = 0;

public:
    friend struct FBuzzzContainerHive;

    UBuzzzContainer();
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

    /**
     * We Should Bind Delegates Inside This
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category="Buzzz")
    void OnInitialization();

private:
    FCriticalSection ContainerCS;

public:
#pragma region Helpers

    UFUNCTION(BlueprintPure, Category="Buzzz")
    int32 GetCapacity() const;

    UFUNCTION(BlueprintPure, Category="Buzzz")
    const TArray<FBuzzzContainerCell>& GetCells() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Buzzz")
    bool CheckItemCompatible(const UBuzzzItem* Item) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    bool CheckItemOwned(const UBuzzzItem* Item) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index"))
    bool CheckCellEmpty(const int32& Index) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index"))
    const FBuzzzContainerCell& GetCell(const int32& Index, bool& IsValidIndex) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta=(ReturnDisplayName="Amount"))
    int32 CalcTotalAmountByInstance(const UBuzzzItem* Item) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta=(ReturnDisplayName="EmptyIndex"))
    int32 FindEmptyCell(bool& Found) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta = (AutoCreateRefTerm = "Index"))
    bool CheckIndexIsValid(const int32& Index) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    void FindIndexByInstance(const UBuzzzItem* Item,
                             TArray<int32>& OutIndexArray,
                             int32& First,
                             int32& Last,
                             bool& Found) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz", meta=(AutoCreateRefTerm="InstanceClass"))
    void FindIndexByInstanceClass(const TSubclassOf<UBuzzzItem>& InstanceClass,
                                  bool Explicit,
                                  TArray<int32>& OutIndexArray,
                                  int32& First,
                                  int32& Last,
                                  bool& Found) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz")
    int32 CalcTotalAmount(UBuzzzItem* Instance);

    UFUNCTION(BlueprintCallable, Category="Buzzz")
    void MarkIndexDirty(const int32 Index, bool bForce = false);

#pragma endregion

#pragma region Client
    /**
     * Client/Standalone Hive Change Deferred Callbacks (Not Triggered on Server)
     *
     * Client: Triggered when the client receives replication data via FastArray callbacks; Mutations are batched.
     * Standalone: Triggered during Next Tick; Mutations are batched.
     */
    UPROPERTY(BlueprintAssignable, Category="Buzzz | Client")
    FBuzzzHiveMutationDelegate Client_ReceiveHiveMutation;
#pragma endregion

private:
#pragma region Internal
    UFUNCTION()
    virtual void Internal_HandlePostCellChanged(const FBuzzzAssignmentContext& Context);

    UFUNCTION()
    virtual void Internal_HandlePostHiveResize(const FBuzzzHiveMutationContext& Context);

    // Internal Used Properties
    // Reset In Each Tick
    UPROPERTY()
    TArray<int32> Internal_Batched_ChangedIndices;

    UPROPERTY()
    TArray<int32> Internal_Batched_AddedIndices;

    UPROPERTY()
    TArray<int32> Internal_Batched_RemovedIndices;

    UPROPERTY()
    TArray<TObjectPtr<UBuzzzItem>> Internal_MayBeDisconnected_Instances;
#pragma endregion


#pragma region Standalone
    /**
     * Only used in Standalone / ListenServer Mode For Submit Batching Indices
     * Ensures consistency with the behavior in C/S Mode
     */
    virtual void Internal_Locally_TrySubmitMutationInfoToClient();

#pragma endregion

public:
#pragma region Internal Authority Delegates

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz | Authority")
    FBuzzzCellMutationDelegate PreCellChange;
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz | Authority")
    FBuzzzCellMutationDelegate PostCellChange;
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz | Authority")
    FBuzzzCellMutationDelegate OnCellChange;
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz | Authority")
    FBuzzzHiveMutationDelegate PostHiveResize;
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category = "Buzzz | Authority")
    FBuzzzCellMutationDelegate OnAssignFailed;

#pragma endregion

#pragma region Capacity
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz", meta = (AutoCreateRefTerm = "NewCapacity"))
    bool Resize(const int32& NewCapacity);
#pragma endregion

#pragma region Assign Operation
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz")
    FBuzzzAssignmentContext AssignCell(UPARAM(ref) FBuzzzAssignmentContext& Context);
#pragma endregion


#pragma region Wrapper Operations
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz",
        meta = (AutoCreateRefTerm = "Index", ReturnDisplayName="Success"))
    FBuzzzAssignmentContext ClearCell(const int32& Index, FBuzzzAssignmentContext& OutContext);
#pragma endregion
};
