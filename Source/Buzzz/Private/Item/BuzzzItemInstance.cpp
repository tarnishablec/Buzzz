// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BuzzzItemInstance.h"
#include "Container/BuzzzContainer.h"
#include "Fragment/BuzzzFragment.h"
#include "Item/BuzzzItemDefinition.h"
#include "Net/UnrealNetwork.h"

UBuzzzItemInstance::UBuzzzItemInstance()
{
    ItemGuid = FGuid::NewGuid();
}

void UBuzzzItemInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ThisClass, Definition);
    DOREPLIFETIME(ThisClass, ItemGuid);
    DOREPLIFETIME(ThisClass, Fragments);
    DOREPLIFETIME(ThisClass, Creator);

    if (const auto BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
    {
        BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
    }
}

int32 UBuzzzItemInstance::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
    return GetOwningContainer()->GetFunctionCallspace(Function, Stack);
}

bool UBuzzzItemInstance::CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams,
                                            FFrame* Stack)
{
    const auto OwningActor = GetOwningContainer()->GetOwner();
    check(OwningActor);

    if (const auto NewDriver = OwningActor->GetNetDriver())
    {
        NewDriver->CallRemoteFunction(Function, Params, OutParams, Stack);
        return true;
    }

    return false;
}

UBuzzzContainer* UBuzzzItemInstance::GetOwningContainer() const
{
    return GetTypedOuter<UBuzzzContainer>();
}

const UObject* UBuzzzItemInstance::GetCreator() const
{
    return Creator;
}

FGuid UBuzzzItemInstance::GetItemGuid() const
{
    return ItemGuid;
}

const UBuzzzItemDefinition* UBuzzzItemInstance::GetDefinition_Implementation() const
{
    return Definition;
}

void UBuzzzItemInstance::Destroy()
{
    if (IsValid(this))
    {
        MarkAsGarbage();
    }
}

void UBuzzzItemInstance::InitializeItemInstance(const UBuzzzItemDefinition* ItemDefinition)
{
    Creator = GetOuter();
    if (!IsValid(ItemDefinition))
    {
        checkNoEntry();
        return;
    }

    Definition = ItemDefinition;
    InitializeFragments();
    OnInitialized();
}

void UBuzzzItemInstance::OnInitialized_Implementation()
{
}

void UBuzzzItemInstance::InitializeFragments()
{
    check(IsValid(Definition));

    Fragments.Empty();

    for (auto&& FragmentTemplate : Definition->FragmentsTemplate)
    {
        if (FragmentTemplate != nullptr)
        {
            const auto Fragment = NewObject<UBuzzzFragment>(this, FragmentTemplate.GetClass(), NAME_None, RF_NoFlags,
                                                            FragmentTemplate);
            Fragments.AddUnique(Fragment);
            Fragment->InitializeFragment(this);
        }
    }
}

const UBuzzzFragment* UBuzzzItemInstance::FindFragmentByClass(const TSubclassOf<UBuzzzFragment>& FragmentClass,
                                                              bool Exact) const
{
    check(IsValid(Definition));

    if (!IsValid(FragmentClass))
    {
        return nullptr;
    }

    return *Fragments.FindByPredicate(
        [Exact,FragmentClass](const TObjectPtr<UBuzzzFragment>& Fragment)
        {
            if (Exact)
            {
                return Fragment->GetClass() == FragmentClass;
            }
            return Fragment.IsA(FragmentClass);
        });
}
