#include <ntifs.h>
#include "tools.h"

VOID DriverUnload(PDRIVER_OBJECT pDriver) {
	KdPrintEx((77,0,"Driver unload.\n"));
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver,PUNICODE_STRING pReg) {
	pDriver->DriverUnload = DriverUnload;
	QueryAllModule();
	return STATUS_SUCCESS;
}