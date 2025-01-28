// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Transaction/BuzzzBridgeBin.h"


// Sets default values for this component's properties
UBuzzzBridgeBin::UBuzzzBridgeBin()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    bReplicateUsingRegisteredSubObjectList = true;
    InitialCapacity = 100;
    // ...
}


// Called when the game starts
void UBuzzzBridgeBin::BeginPlay()
{
    Super::BeginPlay();

    // ...
}


// Called every frame
void UBuzzzBridgeBin::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}
