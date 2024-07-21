#pragma once
#include <stdio.h>
#include "..\..\TestDriver\R0Comnicate\ComnicateStructure.h"  // R3,R0共用一个数据结构(需在R3Test项目属性中 预处理器定义添加 _R3)

BOOLEAN DriverInit();
BOOLEAN DriverComm(ULONG code, PVOID inData, ULONG inSize, PVOID outData, ULONG outSize);