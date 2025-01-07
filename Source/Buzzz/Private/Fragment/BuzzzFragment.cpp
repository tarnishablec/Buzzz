// Fill out your copyright notice in the Description page of Project Settings.


#include "Fragment/BuzzzFragment.h"

#include "Container/BuzzzContainer.h"
#include "Item/BuzzzItemInstance.h"

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
    const auto OwningContainer = GetHostItemInstance()->GetOwningContainer();

    if (OwningContainer)
    {
        const auto OwningActor = OwningContainer->GetOwner();
        check(OwningActor);

        if (const auto NewDriver = OwningActor->GetNetDriver())
        {
            NewDriver->CallRemoteFunction(Function, Params, OutParams, Stack);
            return true;
        }
    }

    return false;
}

void UBuzzzFragment::InitializeFragment(UBuzzzItemInstance* HostItemInstance)
{
    if (!IsValid(HostItemInstance))
    {
        return;
    }

    OnInitialized(HostItemInstance);
}

UBuzzzItemInstance* UBuzzzFragment::GetHostItemInstance() const
{
    return GetTypedOuter<UBuzzzItemInstance>();
}
