// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Helpers/BuzzzLibrary.h"
#include "Container/BuzzzFastArray.h"
#include "Item/BuzzzItemInstance.h"


void UBuzzzLibrary::CellGetContainerInfo(const FBuzzzContainerCell& Cell, UBuzzzContainer*& Container, int32& Index)
{
    Container = Cell.Container;
    Index = Cell.Index;
}
