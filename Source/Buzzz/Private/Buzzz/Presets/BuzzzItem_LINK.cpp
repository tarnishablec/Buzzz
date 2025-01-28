// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Presets/BuzzzItem_LINK.h"

#include "Buzzz/Core/Container/BuzzzContainer.h"
#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Buzzz/Helpers/BuzzzAction_WaitForItemRemovalOrAddition.h"
#include "Net/UnrealNetwork.h"

bool UBuzzzItem_LINK::CheckSourceAccessible_Implementation() const
{
    return IsValid(SourceItem);
}

void UBuzzzItem_LINK::OnRep_SourceItem_Implementation()
{
}

const UBuzzzFragment* UBuzzzItem_LINK::FindFragmentByClass_Implementation(
    const TSubclassOf<UBuzzzFragment> FragmentClass, const bool Exact) const
{
    if (IsValid(SourceItem))
    {
        return SourceItem->FindFragmentByClass(FragmentClass, Exact);
    }
    return nullptr;
}

void UBuzzzItem_LINK::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    constexpr FDoRepLifetimeParams SourceInstanceParam{COND_OwnerOnly, REPNOTIFY_OnChanged, true};
    DOREPLIFETIME_WITH_PARAMS(ThisClass, SourceItem, SourceInstanceParam);
}

void UBuzzzItem_LINK::OnInitialization_Implementation()
{
    check(IsValid(SourceItem));
    Super::OnInitialization_Implementation();

    WaitForSourceRemovalAction = UBuzzzAction_WaitForItemRemovalOrAddition::WaitForRemoval(
        SourceItem, SourceItem);
    WaitForSourceRemovalAction->Triggered.AddDynamic(this, &UBuzzzItem_LINK::HandleSourceRemoval);
    WaitForSourceRemovalAction->Activate();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBuzzzItem_LINK::HandleSourceRemoval(const FBuzzzItemTransferContext& Context)
{
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

void UBuzzzItem_LINK::PreKilled_Implementation()
{
    WaitForSourceRemovalAction->Cancel();
    Super::PreKilled_Implementation();
}
