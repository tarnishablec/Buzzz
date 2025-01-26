// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Beeep/BeeepMessageSubsystem.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "GameFramework/Actor.h"
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

    UPROPERTY()
    TMap<TObjectPtr<UBuzzzItem>, TObjectPtr<UBuzzzContainer>> Recycler;

    UPROPERTY()
    TMap<TSubclassOf<UBuzzzItem>, FItemRegistryEntry> ItemRegistry;

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

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        // DOREPLIFETIME_CONDITION(ThisClass, ItemRegistry, COND_None);
    };
};
