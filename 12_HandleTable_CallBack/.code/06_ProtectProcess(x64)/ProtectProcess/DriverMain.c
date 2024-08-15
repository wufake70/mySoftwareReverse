#include <ntifs.h>
#include "ProcessProtect.h"
#include "Tool.h"

VOID DriverUnload(PDRIVER_OBJECT pDriver);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	pDriver->DriverUnload = DriverUnload;
	PEPROCESS targetPeprocess = NULL;
	targetPeprocess = FindProcessByNameA("notepad.exe");
	if (targetPeprocess) {
		ProtectProcess(PsGetProcessId(targetPeprocess));
		KdPrintEx((77, 0, "start protect target process\n"));
	}
	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	KdPrintEx((77, 0, "DriverUnload\n"));
}