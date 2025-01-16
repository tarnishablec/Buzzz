// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuzzzContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BuzzzSubsystem.generated.h"

class UBuzzzItemInstance;
struct FBuzzzCellOperationContext;
class UBuzzzContainer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBuzzzReceiveInstanceDisconnectDelegate, UBuzzzItemInstance*,
                                             ItemInstance, UBuzzzContainer*, Container);

/**
 * 
 */
UCLASS()
class BUZZZ_API UBuzzzSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzCellMutationDelegate ReceiveContainerCellMutation;

    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzReceiveInstanceDisconnectDelegate ReceiveInstanceDisconnect;
};
