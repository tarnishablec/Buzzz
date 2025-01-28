// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Core/Item/BuzzzItem.h"
#include "Buzzz/Core/Fragment/BuzzzFragment.h"
#include "Net/UnrealNetwork.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif

UBuzzzItem::UBuzzzItem()
{
}

void UBuzzzItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    Params.Condition = COND_InitialOnly;
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

int32 UBuzzzItem::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
    const auto OuterActor = GetTypedOuter<AActor>();
    if (IsValid(OuterActor))
    {
        const auto CallSpace = OuterActor->GetFunctionCallspace(Function, Stack);
        return CallSpace;
    }
    return GEngine->GetGlobalFunctionCallspace(Function, this, Stack);
}

bool UBuzzzItem::CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams,
                                    FFrame* Stack)
{
    if (const auto OwningActor = GetTypedOuter<AActor>())
    {
        if (const auto NewDriver = OwningActor->GetNetDriver())
        {
            NewDriver->ProcessRemoteFunction(OwningActor, Function, Params, OutParams, Stack, this);
            return true;
        }
    }
    return false;
}

void UBuzzzItem::BeginDestroy()
{
    UObject::BeginDestroy();
}

class UWorld* UBuzzzItem::GetWorld() const
{
    return GEngine->GetCurrentPlayWorld();
}

void UBuzzzItem::PreDestroyFromReplication()
{
    UObject::PreDestroyFromReplication();
}

#if UE_WITH_IRIS
void UBuzzzItem::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                              const UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
    using namespace UE::Net;

    FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif

FGuid UBuzzzItem::GetItemGuid() const
{
    return ItemGuid;
}


const UBuzzzFragment* UBuzzzItem::FindFragmentByClass_Implementation(
    const TSubclassOf<UBuzzzFragment> FragmentClass,
    const bool Exact) const
{
    if (IsValid(FragmentClass))
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

void UBuzzzItem::Initialize()
{
    if (bInitialized)
    {
        return;
    }

    InitializeFragments();
    OnInitialization();
    bInitialized = true;
    PostInitialized();
}

void UBuzzzItem::OnInitialization_Implementation()
{
}


void UBuzzzItem::PostInitialized_Implementation()
{
}

void UBuzzzItem::InitializeFragments_Implementation()
{
    for (auto&& Fragment : Fragments)
    {
        Fragment->InitializeFragment();
    }
}

void UBuzzzItem::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& ObjList)
{
    for (auto&& BuzzzFragment : Fragments)
    {
        if (BuzzzFragment->IsSupportedForNetworking())
        {
            ObjList.AddUnique(BuzzzFragment);
        }
    }
}


UBuzzzItem* UBuzzzItem::MakeInstance_Static(
    // ReSharper disable once CppPassValueParameterByConstReference
    const TSubclassOf<UBuzzzItem> ItemClass,
    AActor* Creator)
{
    check(ItemClass);

    if (!IsValid(ItemClass))
    {
        return nullptr;
    }

    const auto FinalInstance = ItemClass.GetDefaultObject()->InstancingMode.GetDefaultObject()->MakeInstance(
        ItemClass, Creator);

    return FinalInstance;
}

void UBuzzzItem::Demolish()
{
    PreDemolish();

    this->ConditionalBeginDestroy();
}

void UBuzzzItem::PreDemolish_Implementation()
{
}
