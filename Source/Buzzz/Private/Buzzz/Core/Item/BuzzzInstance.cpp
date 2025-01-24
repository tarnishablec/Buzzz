// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Core/Item/BuzzzInstance.h"
#include "Buzzz/Core/Fragment/BuzzzFragment.h"
#include "Buzzz/Core/Item/BuzzzDefinition.h"
#include "Net/UnrealNetwork.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif

UBuzzzInstance::UBuzzzInstance()
{
}

void UBuzzzInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    Params.Condition = COND_InitialOnly;
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Definition, Params)
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ItemGuid, Params)
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InstancingMode, Params)

    FDoRepLifetimeParams FragmentsParams;
    FragmentsParams.bIsPushBased = true;
    FragmentsParams.Condition = COND_OwnerOnly;
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Fragments, FragmentsParams)

    if (const auto BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
    {
        BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
    }
}

int32 UBuzzzInstance::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
    const auto OuterActor = GetTypedOuter<AActor>();
    if (IsValid(OuterActor))
    {
        const auto CallSpace = GetTypedOuter<AActor>()->GetFunctionCallspace(Function, Stack);
        return CallSpace;
    }
    return FunctionCallspace::Local;
}

bool UBuzzzInstance::CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams,
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

void UBuzzzInstance::BeginDestroy()
{
    UObject::BeginDestroy();
}

class UWorld* UBuzzzInstance::GetWorld() const
{
    return GEngine->GetCurrentPlayWorld();
}

#if UE_WITH_IRIS
void UBuzzzInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                                  const UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
    using namespace UE::Net;

    FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif

FGuid UBuzzzInstance::GetItemGuid() const
{
    return ItemGuid;
}

const UBuzzzFragment* UBuzzzInstance::FindFragmentByClass_Implementation(
    const TSubclassOf<UBuzzzFragment> FragmentClass,
    const bool Exact) const
{
    if (IsValid(Definition) && IsValid(FragmentClass))
    {
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
    }
    return nullptr;
}

const UBuzzzDefinition* UBuzzzInstance::GetDefinition_Implementation() const
{
    return Definition;
}

void UBuzzzInstance::Initialize_Implementation()
{
    check(!bInitialized);
    InitializeFragments();
    OnInitialization();
    bInitialized = true;
}

void UBuzzzInstance::OnInitialization_Implementation()
{
}

void UBuzzzInstance::InitializeFragments_Implementation()
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

void UBuzzzInstance::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& ObjList)
{
    for (auto&& BuzzzFragment : Fragments)
    {
        if (BuzzzFragment->IsSupportedForNetworking())
        {
            ObjList.AddUnique(BuzzzFragment);
        }
    }
}
