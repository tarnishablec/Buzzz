// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Presets/BuzzzInstance_LINK.h"

#include "Core/Container/BuzzzContainer.h"
#include "Core/Item/BuzzzInstance.h"
#include "Net/UnrealNetwork.h"

bool UBuzzzInstance_LINK::CheckSourceValid_Implementation() const
{
    return IsValid(SourceInstance);
}

const UBuzzzFragment* UBuzzzInstance_LINK::FindFragmentByClass_Implementation(
    const TSubclassOf<UBuzzzFragment> FragmentClass, const bool Exact) const
{
    if (IsValid(SourceInstance))
    {
        return SourceInstance->FindFragmentByClass(FragmentClass, Exact);
    }
    return nullptr;
}

void UBuzzzInstance_LINK::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    constexpr FDoRepLifetimeParams SourceInstanceParam{COND_InitialOnly, REPNOTIFY_OnChanged, true};
    DOREPLIFETIME_WITH_PARAMS(ThisClass, SourceInstance, SourceInstanceParam);
    constexpr FDoRepLifetimeParams IsSourceValidParam{COND_OwnerOnly, REPNOTIFY_OnChanged, true};
    DOREPLIFETIME_WITH_PARAMS(ThisClass, IsSourceValid, IsSourceValidParam);
}

void UBuzzzInstance_LINK::HandleSourceDisconnect(UBuzzzInstance* ItemInstance, const UBuzzzContainer* Container)
{
    Client_ReceiveSourceDisconnect(ItemInstance, Container);
}

void UBuzzzInstance_LINK::Client_ReceiveSourceDisconnect_Implementation(
    UBuzzzInstance* InSourceInstance, const UBuzzzContainer* Container)
{
    check(Container->IsNetMode(NM_Client));
}

void UBuzzzInstance_LINK::OnInitialization_Implementation()
{
    check(IsValid(SourceInstance));


    Super::OnInitialization_Implementation();
}
