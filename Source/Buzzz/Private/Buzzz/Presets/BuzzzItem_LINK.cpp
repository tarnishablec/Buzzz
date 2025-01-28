// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Presets/BuzzzItem_LINK.h"

#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Buzzz/Helpers/BuzzzAction_WaitForItemRemovalOrAddition.h"
#include "Net/UnrealNetwork.h"

bool UBuzzzItem_LINK::CheckSourceValid_Implementation() const
{
    return IsValid(SourceInstance);
}

void UBuzzzItem_LINK::OnRep_IsSourceValid()
{
}

const UBuzzzFragment* UBuzzzItem_LINK::FindFragmentByClass_Implementation(
    const TSubclassOf<UBuzzzFragment> FragmentClass, const bool Exact) const
{
    if (IsValid(SourceInstance))
    {
        return SourceInstance->FindFragmentByClass(FragmentClass, Exact);
    }
    return nullptr;
}

void UBuzzzItem_LINK::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    constexpr FDoRepLifetimeParams SourceInstanceParam{COND_InitialOnly, REPNOTIFY_OnChanged, true};
    DOREPLIFETIME_WITH_PARAMS(ThisClass, SourceInstance, SourceInstanceParam);
    constexpr FDoRepLifetimeParams IsSourceValidParam{COND_OwnerOnly, REPNOTIFY_OnChanged, true};
    DOREPLIFETIME_WITH_PARAMS(ThisClass, IsSourceValid, IsSourceValidParam);
}

void UBuzzzItem_LINK::OnInitialization_Implementation()
{
    check(IsValid(SourceInstance));
    Super::OnInitialization_Implementation();

    IsSourceValid = CheckSourceValid();

    WaitForSourceRemovalAction = UBuzzzAction_WaitForItemRemovalOrAddition::WaitForRemoval(
        SourceInstance, SourceInstance);

    WaitForSourceRemovalAction->Triggered.AddDynamic(this, &UBuzzzItem_LINK::HandleSourceRemoval);
    WaitForSourceRemovalAction->Activate();
}

void UBuzzzItem_LINK::HandleSourceRemoval(const FBuzzzItemTransferContext& Context)
{
    IsSourceValid = CheckSourceValid();
    const auto OwnerContainer = Cast<UBuzzzContainer>(GetOuter());
    if (IsValid(OwnerContainer))
    {
        TArray<int32> OutIndices;
        int32 OutFirst;
        int32 OutLast;
        bool OutFound;
        OwnerContainer->FindIndexByInstance(this, OutIndices, OutFirst, OutLast, OutFound);
        if (OutFound)
        {
            OwnerContainer->MarkIndexDirty(OutFirst, true);
        }
    }
}

void UBuzzzItem_LINK::PreDemolish_Implementation()
{
    WaitForSourceRemovalAction->Cancel();
    Super::PreDemolish_Implementation();
}
