// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Item/BuzzzItemInstance_LINK.h"

#include "Helpers/BuzzzAction_WaitForInstanceDisconnect.h"
#include "Net/UnrealNetwork.h"

bool UBuzzzItemInstance_LINK::CheckSourceValid_Implementation() const
{
    return IsValid(SourceInstance);
}

UBuzzzItemInstance* UBuzzzItemInstance_LINK::MakeInstance_Implementation(const UBuzzzItemDefinition* InDefinition,
                                                                         AActor* Instigator) const
{
    return NewObject<UBuzzzItemInstance_LINK>(GetTransientPackage(), GetClass());
}

const UBuzzzFragment* UBuzzzItemInstance_LINK::FindFragmentByClass_Implementation(
    const TSubclassOf<UBuzzzFragment> FragmentClass, const bool Exact) const
{
    return SourceInstance->FindFragmentByClass(FragmentClass, Exact);
}

void UBuzzzItemInstance_LINK::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    constexpr FDoRepLifetimeParams SourceInstanceParam{COND_InitialOnly, REPNOTIFY_OnChanged, true};
    DOREPLIFETIME_WITH_PARAMS(ThisClass, SourceInstance, SourceInstanceParam);
    constexpr FDoRepLifetimeParams IsSourceValidParam{COND_OwnerOnly, REPNOTIFY_OnChanged, true};
    DOREPLIFETIME_WITH_PARAMS(ThisClass, IsSourceValid, IsSourceValidParam);
}

void UBuzzzItemInstance_LINK::HandleSourceDisconnect(UBuzzzItemInstance* ItemInstance, const UBuzzzContainer* Container)
{
    Client_ReceiveSourceDisconnect(ItemInstance, Container);
}

void UBuzzzItemInstance_LINK::Client_ReceiveSourceDisconnect_Implementation(
    UBuzzzItemInstance* InSourceInstance, const UBuzzzContainer* Container)
{
    check(Container->IsNetMode(NM_Client));
}

void UBuzzzItemInstance_LINK::OnInitialization_Implementation()
{
    check(IsValid(SourceInstance));


    Super::OnInitialization_Implementation();
}
