#pragma once
#include <stdio.h>
#include "..\..\TestDriver\R0Comnicate\ComnicateStructure.h"  // R3,R0����һ�����ݽṹ(����R3Test��Ŀ������ Ԥ������������� _R3)

BOOLEAN DriverInit();
BOOLEAN DriverComm(ULONG code, PVOID inData, ULONG inSize, PVOID outData, ULONG outSize);