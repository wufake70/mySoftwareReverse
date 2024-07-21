#include <ntifs.h>
#include "R0Comnicate/R0Comnicate.h"


NTSTATUS NTAPI Dispatch(PCommPackage package)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	switch (package->code){
	case CODE_READ:
	{
		PTest t = (PTest)package->inData;
		memset(t,0x11,sizeof(t));
		break;
	}
	case CODE_WRITE:
	{
		PTest t = (PTest)package->inData;
		KdPrintEx((77,0,"CODE_WRITE: 0x%x\n",t->x));
		break;
	}
	}

	return status;
}

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	DriverDestoryComm(pDriver);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	// 只需要提供消息派遣的回调函数即可
	DriverRegisterComm(pDriver, Dispatch);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}