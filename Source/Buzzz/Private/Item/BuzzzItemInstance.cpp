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

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    Params.Condition = COND_InitialOnly;
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Definition, Params)
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ItemGuid, Params)

    FDoRepLifetimeParams FragmentsParams;
    FragmentsParams.bIsPushBased = true;
    FragmentsParams.Condition = COND_OwnerOnly;
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Fragments, FragmentsParams)

    if (const auto BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
    {
        BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
    }
}

int32 UBuzzzItemInstance::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
    const auto OuterActor = GetTypedOuter<AActor>();
    if (IsValid(OuterActor))
    {
        const auto CallSpace = GetTypedOuter<AActor>()->GetFunctionCallspace(Function, Stack);
        return CallSpace;
    }
    return FunctionCallspace::Local;
}

bool UBuzzzItemInstance::CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams,
                                            FFrame* Stack)
{
    if (const auto OwningActor = GetTypedOuter<AActor>())
    {
        if (const auto NewDriver = OwningActor->GetNetDriver())
        {
            return NewDriver->CallRemoteFunction(Function, Params, OutParams, Stack);
        }
    }
    return false;
}

void UBuzzzItemInstance::BeginDestroy()
{
    UObject::BeginDestroy();
}

class UWorld* UBuzzzItemInstance::GetWorld() const
{
    return  UObject::GetWorld();
}

#if UE_WITH_IRIS
void UBuzzzItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                                      const UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
    using namespace UE::Net;

    FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif

FGuid UBuzzzItemInstance::GetItemGuid() const
{
    return ItemGuid;
}

UBuzzzItemInstance* UBuzzzItemInstance::MakeInstanceFromDefinition(const UBuzzzItemDefinition* InDefinition,
                                                                   AActor* Instigator)
{
    check(InDefinition);
    return InDefinition->InstanceClass.GetDefaultObject()->MakeInstance(InDefinition, Instigator);
}

// ReSharper disable once CppPassValueParameterByConstReference
UBuzzzItemInstance* UBuzzzItemInstance::MakeInstanceFromClass(const TSubclassOf<UBuzzzItemInstance> InstanceClass,
                                                              AActor* Instigator)
{
    return InstanceClass.GetDefaultObject()->MakeInstance(nullptr, Instigator);
}

const UBuzzzFragment* UBuzzzItemInstance::FindFragmentByClass_Implementation(
    const TSubclassOf<UBuzzzFragment> FragmentClass,
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

const UBuzzzItemDefinition* UBuzzzItemInstance::GetDefinition_Implementation() const
{
    return Definition;
}

void UBuzzzItemInstance::Initialize_Implementation()
{
    check(!bInitialized);
    InitializeFragments();
    OnInitialization();
    bInitialized = true;
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


void UBuzzzItemInstance::InitializeFragments_Implementation()
{
    if (IsValid(Definition))
    {
        Fragments.Empty();

        for (auto&& FragmentTemplate : Definition->FragmentsTemplate)
        {
            if (FragmentTemplate != nullptr)
            {
                // const auto Fragment = DuplicateObject(FragmentTemplate, this);
                const auto Fragment = NewObject<UBuzzzFragment>(this, FragmentTemplate.GetClass(), NAME_None,
                                                                RF_NoFlags,
                                                                FragmentTemplate);
                Fragments.AddUnique(Fragment);
                Fragment->InitializeFragment();
            }
        }
    }
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
