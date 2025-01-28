// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Buzzz/Helpers/BuzzzSharedTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Buzzz/Transaction/BuzzzBridge.h"
#include "BuzzzSubsystem.generated.h"

class ABuzzzManager;
struct FInstancedStruct;
class ABuzzzBridge;
class UBuzzzTransaction;
class UBuzzzItem;
struct FBuzzzAssignmentContext;
class UBuzzzContainer;


template <typename T>
concept pointer_convertible_to_buzzz_item_instance =
    std::is_convertible_v<T*, UBuzzzItem*>;

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


    UFUNCTION(BlueprintCallable, Category="Buzzz")
    void RegisterInstance(UBuzzzItem* Instance);
    bool CheckInstanceRegistered(const UBuzzzItem* Instance) const;

    UFUNCTION(BlueprintCallable, Category="Buzzz", meta=(DeterminesOutputType="InstanceClass"))
    UBuzzzItem* Instantiate(
        UPARAM(meta=(AllowAbstract=false)) TSubclassOf<UBuzzzItem> InstanceClass,
        AActor* Creator);

    template <pointer_convertible_to_buzzz_item_instance T>
    T* Instantiate(
        AActor* Creator)
    {
        return Instantiate(T::StaticClass(), Creator);
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
    void RegisterBridgeLink(APlayerController* PlayerController, ABuzzzBridge* Bridge);
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

private:
    bool bInitialized = false;

    TObjectPtr<ABuzzzManager> Manager;

    TMap<TWeakObjectPtr<APlayerController>, TWeakObjectPtr<ABuzzzBridge>> BridgeRegistry;

};
