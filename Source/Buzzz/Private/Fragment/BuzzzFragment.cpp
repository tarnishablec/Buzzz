// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Fragment/BuzzzFragment.h"
#include "Item/BuzzzItemInstance.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif

void UBuzzzFragment::OnInitialized_Implementation(UBuzzzItemInstance* ItemInstance)
{
}

void UBuzzzFragment::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
    if (const auto BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
    {
        BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
    }
}

UWorld* UBuzzzFragment::GetWorld() const
{
    return UObject::GetWorld();
}

int32 UBuzzzFragment::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
    return GetHostItemInstance()->GetFunctionCallspace(Function, Stack);
}

bool UBuzzzFragment::CallRemoteFunction(UFunction* Function, void* Params, struct FOutParmRec* OutParams, FFrame* Stack)
{
    return GetHostItemInstance()->CallRemoteFunction(Function, Params, OutParams, Stack);
}

#if UE_WITH_IRIS
void UBuzzzFragment::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                                  UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
    UE::Net::FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif

void UBuzzzFragment::InitializeFragment()
{
    OnInitialized(GetHostItemInstance());
}

void UBuzzzFragment::OnDestroy_Implementation()
{
}

UBuzzzItemInstance* UBuzzzFragment::GetHostItemInstance() const
{
    return GetTypedOuter<UBuzzzItemInstance>();
}
