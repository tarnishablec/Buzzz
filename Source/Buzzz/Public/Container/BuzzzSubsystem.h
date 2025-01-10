// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BuzzzSubsystem.generated.h"

struct FBuzzzOperationContext;
class UBuzzzContainer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuzzzSubsystemReceivedContainerMutation, const FBuzzzOperationContext&,
                                            Context);

/**
 * 
 */
UCLASS()
class BUZZZ_API UBuzzzSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, BlueprintAuthorityOnly, Category="Buzzz")
    FBuzzzSubsystemReceivedContainerMutation ReceivedContainerMutation;
};
