#pragma once
#include <ntifs.h>
#include "ComnicateStructure.h"



// ҵ������
typedef NTSTATUS(NTAPI * CommCallback)(PCommPackage package);

NTSTATUS DriverRegisterComm(PDRIVER_OBJECT  pDriver,CommCallback  callback);

VOID DriverDestoryComm(PDRIVER_OBJECT  pDriver);