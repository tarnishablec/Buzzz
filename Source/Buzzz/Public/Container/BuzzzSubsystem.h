// Copyright 2019-Present tarnishablec. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BuzzzSubsystem.generated.h"

struct FBuzzzCellOperationContext;
class UBuzzzContainer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuzzzSubsystemReceivedContainerMutation, const FBuzzzCellOperationContext&,
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
