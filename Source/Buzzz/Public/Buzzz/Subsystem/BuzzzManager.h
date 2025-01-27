// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
// #include "Net/UnrealNetwork.h"
#include "BuzzzManager.generated.h"

struct FItemRegistryEntry;
class UBuzzzItem;
class UBuzzzRecycler;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_BuzzzEvent_ItemDisconnect);

USTRUCT()
struct FItemRegistryEntry
{
    GENERATED_BODY()

    UPROPERTY()
    TSet<TObjectPtr<UBuzzzItem>> InstanceSet;
};

UCLASS(Blueprintable)
class BUZZZ_API ABuzzzManager : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABuzzzManager();

    struct FBuzzzRecycler
    {
        struct FBuzzzRecyclerEntry
        {
            TMap<TWeakObjectPtr<UBuzzzItem>, int32> ItemCountMap;
        };

        TMap<TWeakObjectPtr<UBuzzzContainer>, FBuzzzRecyclerEntry> ContainerMap;

        void Throw(UBuzzzItem* Item, UBuzzzContainer* Container)
        {
            auto& [CountMap] = ContainerMap.FindOrAdd(Container);
            const auto CurrentCount = CountMap.FindOrAdd(Item);
            CountMap.Add(Item, CurrentCount + 1);
        }

        void Pick(UBuzzzItem* Item, UBuzzzContainer* Container)
        {
            auto& [CountMap] = ContainerMap.FindOrAdd(Container);
            const auto CurrentCount = CountMap.FindOrAdd(Item);
            CountMap.Add(Item, CurrentCount - 1);
        }

        void Reset()
        {
            ContainerMap.Reset();
        }
    };

    FBuzzzRecycler Recycler;

    UPROPERTY()
    TMap<TSubclassOf<UBuzzzItem>, FItemRegistryEntry> ItemRegistry;

    UPROPERTY(Replicated)
    FGuid ManagerGuid;

    UFUNCTION(BlueprintNativeEvent, Category="Buzzz|Manager")
    void ReceivedCellMutation(const FBuzzzCellAssignmentContext& Context);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    FBeeepMessageListenerHandle CellMutationListenerHandle;

    HIDE_ACTOR_TRANSFORM_FUNCTIONS();

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    virtual void BeginDestroy() override;

    virtual void PostInitializeComponents() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME_CONDITION(ThisClass, ManagerGuid, COND_InitialOnly);
    };
};
