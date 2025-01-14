// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Item/BuzzzItemInstance.h"
#include "Fragment/BuzzzFragment.h"
#include "Item/BuzzzItemDefinition.h"
#include "Net/UnrealNetwork.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif 

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

    if (const auto BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
    {
        BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
    }
}

int32 UBuzzzItemInstance::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
    return GetOuter()->GetFunctionCallspace(Function, Stack);
}

bool UBuzzzItemInstance::CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams,
                                            FFrame* Stack)
{
    const auto OwningActor = GetTypedOuter<AActor>();
    check(OwningActor);

    if (const auto NewDriver = OwningActor->GetNetDriver())
    {
        NewDriver->CallRemoteFunction(Function, Params, OutParams, Stack);
        return true;
    }

    return false;
}

#if UE_WITH_IRIS
void UBuzzzItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                                      UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
    UE::Net::FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif

FGuid UBuzzzItemInstance::GetItemGuid() const
{
    return ItemGuid;
}

const UBuzzzItemDefinition* UBuzzzItemInstance::GetDefinition_Implementation() const
{
    return Definition;
}


// ReSharper disable once CppPassValueParameterByConstReference
const UBuzzzFragment* UBuzzzItemInstance::FindFragmentByClass(TSubclassOf<UBuzzzFragment> FragmentClass,
                                                              const bool Exact) const
{
    check(IsValid(Definition));

    if (!IsValid(FragmentClass))
    {
        return nullptr;
    }

    for (auto&& Fragment : Fragments)
    {
        if (!IsValid(Fragment))
        {
            continue;
        }

        if (Exact)
        {
            if (Fragment->GetClass() == FragmentClass)
            {
                return Fragment;
            }
        }
        else
        {
            if (Fragment.IsA(FragmentClass))
            {
                return Fragment;
            }
        }
    }

    return nullptr;
}

AActor* UBuzzzItemInstance::GetOwnerActor_Implementation() const
{
    return GetTypedOuter<AActor>();
}

void UBuzzzItemInstance::OnInitialization_Implementation()
{
}


UBuzzzItemInstance* UBuzzzItemInstance::MakeInstance_Implementation(const UBuzzzItemDefinition* InDefinition,
                                                                    AActor* Instigator) const
{
    checkNoEntry();
    return nullptr;
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
            Fragment->InitializeFragment();
        }
    }
}

void UBuzzzItemInstance::InitializeInstance()
{
    InitializeFragments();
    OnInitialization();
    bInitialized = true;
}

void UBuzzzItemInstance::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& ObjList)
{
    for (auto&& BuzzzFragment : Fragments)
    {
        if (BuzzzFragment->IsSupportedForNetworking())
        {
            ObjList.AddUnique(BuzzzFragment);
        }
    }
}
