// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Buzzz/Transaction/BuzzzTransactionBridge.h"
#include "BuzzzSubsystem.generated.h"

class UBuzzzInstancingMode;
class UBuzzzDefinition;
struct FInstancedStruct;
class ABuzzzTransactionBridge;
class UBuzzzTransaction;
class UBuzzzInstance;
struct FBuzzzCellAssignmentContext;
class UBuzzzContainer;


template <typename T>
concept pointer_convertible_to_buzzz_item_instance =
    std::is_convertible_v<T*, UBuzzzInstance*>;

/**
 * 
 */
UCLASS()
class BUZZZ_API UBuzzzSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzCellMutationDelegate ReceiveContainerCellMutation;

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzInstanceDisconnectDelegate ReceiveInstanceDisconnect;

    UFUNCTION(BlueprintCallable, Category="Buzzz")
    void RegisterInstance(UBuzzzInstance* Instance);
    bool CheckInstanceRegistered(const UBuzzzInstance* Instance) const;

    UFUNCTION(BlueprintPure, Category="Buzzz")
    TSet<UBuzzzInstance*> GetInstancesByDefinition(const UBuzzzDefinition* Definition) const;

    UFUNCTION(BlueprintCallable, Category="Buzzz", meta=(DeterminesOutputType="InstanceClass"))
    UBuzzzInstance* Instantiate(const UBuzzzDefinition* Definition,
                                TSubclassOf<UBuzzzInstance> InstanceClass,
                                AActor* Creator);

    template <pointer_convertible_to_buzzz_item_instance T>
    T* Instantiate(const UBuzzzDefinition* Definition,
                   AActor* Creator)
    {
        return Instantiate(Definition, T::StaticClass(), Creator);
    };

    static UBuzzzSubsystem* Get(const UObject* ContextObject)
    {
        const auto World = GEngine->GetWorldFromContextObjectChecked(ContextObject);
        const auto Subsystem = World->GetGameInstance()->GetSubsystem<UBuzzzSubsystem>();
        check(Subsystem);
        return Subsystem;
    }

    UFUNCTION(BlueprintCallable, Category="Buzzz", BlueprintCosmetic,
        meta = (AutoCreateRefTerm="TransactionClass,Payload"))
    void Try_Process_Server_Transaction(
        UPARAM(ref) APlayerController*& Instigator,
        UPARAM(meta=(AllowAbstract=false)) const TSubclassOf<UBuzzzTransaction>& TransactionClass,
        const FInstancedStruct& Payload
    );

    // This Will Run Both in Server And Client
    void RegisterBridgeLink(APlayerController* PlayerController, ABuzzzTransactionBridge* Bridge);
    void UnregisterBridgeLink(AActor* PlayerControllerOrBridge);

#pragma region Tickable
    virtual void Tick(float DeltaTime) override;
    virtual UWorld* GetTickableGameObjectWorld() const override;
    virtual ETickableTickType GetTickableTickType() const override;
    virtual TStatId GetStatId() const override { return Super::GetStatID(); };
    virtual bool IsTickable() const override { return !IsTemplate() && bInitialized; };
    virtual bool IsTickableWhenPaused() const override { return true; };
#pragma endregion

    UFUNCTION(BlueprintPure, Category="Buzzz")
    bool HasAuthority() const;

protected:
    bool bInitialized = false;

    struct FItemRegistryEntry
    {
        TSet<UBuzzzInstance*> InstanceSet;
    };

    // Key : DefinitionGuid
    TMap<FGuid, FItemRegistryEntry> ItemRegistry;
    TMap<TWeakObjectPtr<APlayerController>, TWeakObjectPtr<ABuzzzTransactionBridge>> BridgeRegistry;
};
