#include <ntifs.h>
#include "Tool.h"



PWCH WhiteList[] = {
	L"System",
	L"smss.exe",
	L"csrss.exe",
	L"wininit.exe",
	L"services.exe",
	L"lsass.exe",
	L"lsm.exe",
	L"svchost.exe",
	L"dllhost.exe",
	L"WmiPrvSE.exe",
	NULL
};

VOID DriverUnload(PDRIVER_OBJECT pDriver);


VOID SystemThreadRoutine(PVOID Parameter)
{
	// 需要保护的进程
	PEPROCESS TargetProcess = FindProcessByNameA("explorer.exe");
	LARGE_INTEGER Timeout;

	while (TargetProcess)
	{	
		ForceEnumProcess(WhiteList, TargetProcess);

		Timeout.QuadPart = -10000*5000; // 等待 5秒钟
		// 等待一段时间，让线程执行一些操作
		KeDelayExecutionThread(KernelMode, FALSE, &Timeout);

		TargetProcess = FindProcessByNameA("explorer.exe");
	}
	KdPrintEx((77,0,"Thread exit\n"));
	PsTerminateSystemThread(STATUS_SUCCESS);
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg) {
	pDriver->DriverUnload = DriverUnload;

	HANDLE ThreadHandle = NULL;

	// 需要保护的进程
	PEPROCESS TargetProcess = FindProcessByNameA("explorer.exe");
	if (TargetProcess) {
		NTSTATUS status = PsCreateSystemThread(&ThreadHandle, THREAD_ALL_ACCESS, NULL, NULL, NULL, SystemThreadRoutine, NULL);
	}else {
		KdPrintEx((77, 0, "not exist target process\n"));
	}
	return STATUS_SUCCESS;
}

