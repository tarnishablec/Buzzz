// Copyright 2019-Present tarnishablec. All Rights Reserved.


#include "Buzzz/Helpers/BuzzzLibrary.h"
#include "Buzzz/Core/Container/BuzzzFastArray.h"


void UBuzzzLibrary::CellGetContainerInfo(const FBuzzzContainerCell& Cell, UBuzzzContainer*& Container, int32& Index)
{
    Container = Cell.Container;
    Index = Cell.Index;
}
