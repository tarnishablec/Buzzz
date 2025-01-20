// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StructUtils/InstancedStruct.h"
#include "BuzzzTransactionBridge.generated.h"

struct FInstancedStruct;
class UBuzzzTransaction;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuzzzReceiveTransactionDelegate, const UBuzzzTransaction* const,
                                            Transaction);


template <typename T>
concept pointer_convertible_to_buzzz_transaction =
    std::is_convertible_v<T*, UBuzzzTransaction*>;


UCLASS(Blueprintable, NotPlaceable)
class BUZZZ_API ABuzzzTransactionBridge : public AActor
{
    GENERATED_BODY()

public:
    ABuzzzTransactionBridge();
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void SetOwner(AActor* NewOwner) override;

#if UE_WITH_IRIS
    virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                              UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif

    //

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzReceiveTransactionDelegate ReceiveTransactionStarted;

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzReceiveTransactionDelegate ReceiveTransactionFinished;
    
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzReceiveTransactionDelegate ReceiveTransactionFailed;

private:
    HIDE_ACTOR_TRANSFORM_FUNCTIONS();

protected:
    UPROPERTY(ReplicatedUsing = OnRep_OwnerPlayerController)
    TObjectPtr<APlayerController> OwnerPlayerController;

    UFUNCTION()
    void OnRep_OwnerPlayerController();

public:
    UFUNCTION(Server, Reliable, meta=(AutoCreateRefTerm="Payload"))
    void Server_ProcessTransaction(
        TSubclassOf<UBuzzzTransaction> TransactionClass,
        const FInstancedStruct& Payload
    );

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category="Buzzz",
        meta=(AutoCreateRefTerm="Payload", DeterminesOutputType="TransactionClass"))
    UBuzzzTransaction* ProcessTransactionByClass(
        UPARAM(meta=(AllowAbstract=false)) TSubclassOf<UBuzzzTransaction> TransactionClass,
        const FInstancedStruct& Payload = FInstancedStruct());


    template <pointer_convertible_to_buzzz_transaction T>
    T* ProcessTransactionByClass(const FInstancedStruct& Payload = FInstancedStruct())
    {
        return CastChecked<T>(ProcessTransactionByClass(T::StaticClass(), Payload));
    }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category="Buzzz")
    UBuzzzTransaction* MakeTransaction(
        TSubclassOf<UBuzzzTransaction> TransactionClass,
        const FInstancedStruct& Payload
    );
};
