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
struct BUZZZ_API FBuzzzContainerAssignOperationContext
{
    GENERATED_BODY()

    FBuzzzContainerAssignOperationContext()
    {
        ContainerOperationId = FGuid::NewGuid();
    }

    FGuid ContainerOperationId{};

    UPROPERTY(BlueprintReadWrite)
    int32 TargetIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBuzzzItemInstance> ItemInstance;

    UPROPERTY(BlueprintReadWrite)
    int32 FinalStackCount = 0;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBuzzzContainer> TargetContainer;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBuzzzContainer> SourceContainer;

    UPROPERTY(BlueprintReadWrite)
    int32 SourceIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly)
    FBuzzzContainerCell PreviousCellInfo{};

    UPROPERTY(BlueprintReadOnly)
    bool bSuccess = false;

    UPROPERTY(BlueprintReadOnly)
    bool bFinished = false;
};


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
    // virtual void BeginReplication() override;
    virtual void InitializeComponent() override;
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    FCriticalSection ContainerCS;

public:
    // Sets default values for this component's properties
    UBuzzzContainer();

#pragma region Helpers
    UFUNCTION(BlueprintNativeEvent, Category = "Buzzz | Container", BlueprintPure)
    bool CheckItemCompatible(UBuzzzItemInstance* ItemInstance) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz | Container")
    bool CheckItemInstanceOwned(UBuzzzItemInstance* ItemInstance) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz | Container", meta = (AutoCreateRefTerm = "Index"))
    bool CheckCellEmpty(const int32& Index) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz | Container", meta = (AutoCreateRefTerm = "Index"))
    const FBuzzzContainerCell& GetCell(const int32& Index, bool& IsValidIndex) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz | Container", meta = (AutoCreateRefTerm = "Index"))
    bool CheckCellHasItem(const int32& Index, const UBuzzzItemDefinition* ItemDefinition) const;

    UFUNCTION(BlueprintPure, Category = "Buzzz | Container", meta=(ReturnDisplayName="Amount"))
    int32 CalcTotalAmountOfItem(const UBuzzzItemDefinition* ItemDefinition);

    UFUNCTION(BlueprintPure, Category = "Buzzz | Container", meta=(ReturnDisplayName="EmptyIndex"))
    int32 FindEmptySlot(bool& Found) const;
#pragma endregion Helpers


    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Buzzz | Container",
        meta = (AutoCreateRefTerm = "Context"))
    void AssignCell(UPARAM(ref) FBuzzzContainerAssignOperationContext& Context,
                    FBuzzzContainerAssignOperationContext& OutContext);
};
