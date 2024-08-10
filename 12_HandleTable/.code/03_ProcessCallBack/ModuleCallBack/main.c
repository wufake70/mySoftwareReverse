#include <ntifs.h>
#include <ntstrsafe.h>



VOID LoadImageCallBack(
	_In_opt_ PUNICODE_STRING FullImageName,// 模块的全路径
	_In_ HANDLE ProcessId,                //  pid = 0/4
	_In_ PIMAGE_INFO ImageInfo
	);
VOID DriverUnload(PDRIVER_OBJECT pDriver);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg) {
	pDriver->DriverUnload = DriverUnload;


	NTSTATUS status = PsSetLoadImageNotifyRoutine(LoadImageCallBack);
	
	if (NT_SUCCESS(status)) {
		KdPrintEx((77, 0, "LoadImageCallBack register ok\n"));
	}
	else
	{

		KdPrintEx((77, 0, "LoadImageCallBack register fail\nCode: %x", status));
	}

	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT pDriver) {
	// 移除回调函数
	PsRemoveLoadImageNotifyRoutine(LoadImageCallBack);
	KdPrintEx((77, 0, "DriverUnload\n"));
}


VOID LoadImageCallBack(
	_In_opt_ PUNICODE_STRING FullImageName,
	_In_ HANDLE ProcessId,                // pid into which image is being mapped
	_In_ PIMAGE_INFO ImageInfo
){
	if (ImageInfo->SystemModeImage == 1) {	// sys
		KdPrintEx((77,0,"[dbg] 驱动加载, %wZ\n",FullImageName));

	}
	else
	{
		KdPrintEx((77,0,"[dbg] dll加载, %wZ\n",FullImageName));

	}
}