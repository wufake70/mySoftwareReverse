#include <ntifs.h>

VOID CreateThreadCallBack(
	_In_ HANDLE ProcessId,
	_In_ HANDLE ThreadId,
	_In_ BOOLEAN Create
	);
VOID DriverUnload(PDRIVER_OBJECT pDriver);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg) {
	pDriver->DriverUnload = DriverUnload;

	NTSTATUS status = PsSetCreateThreadNotifyRoutine(CreateThreadCallBack);
	if (NT_SUCCESS(status)) {
		KdPrintEx((77, 0, "CreateThreadCallBack register ok\n"));
	}
	else
	{

		KdPrintEx((77, 0, "CreateThreadCallBack register fail\nCode: %x", status));
	}

	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT pDriver) {
	// ÒÆ³ý»Øµ÷º¯Êý
	PsRemoveCreateThreadNotifyRoutine(CreateThreadCallBack);
	KdPrintEx((77, 0, "DriverUnload\n"));
}


VOID CreateThreadCallBack(
	_In_ HANDLE ProcessId,
	_In_ HANDLE ThreadId,
	_In_ BOOLEAN Create
) {
	PEPROCESS pEprocess = NULL;
	NTSTATUS status = PsLookupProcessByProcessId(ProcessId,&pEprocess);
	ObDereferenceObject(pEprocess);
	if (NT_SUCCESS(status));
	if (Create) {

		KdPrintEx((77, 0, "[dbg] pid:%d(%s),new tid:%d\n", ProcessId, (ULONG_PTR)pEprocess + 0x16c, ThreadId));
	}else {
		KdPrintEx((77, 0, "[dbg] pid:%d(%s),tid:%d die\n", ProcessId, (ULONG_PTR)pEprocess + 0x16c, ThreadId));

	}
}