#include <ntifs.h>

// 需要测试的驱动

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{

}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	DbgPrintEx(77, 0, "-----------Run Successfully.--------------\r\n");
	//pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}