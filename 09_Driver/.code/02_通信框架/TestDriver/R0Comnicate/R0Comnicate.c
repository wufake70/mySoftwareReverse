#include "R0Comnicate.h"

#define DEVICE_NAME L"\\Device\\hackflame1"

CommCallback gCommCallback = NULL;

NTSTATUS DefDispatch(DEVICE_OBJECT *DeviceObject, IRP *Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, 0);
	return STATUS_SUCCESS;
}

// 写的派发函数
NTSTATUS WriteDispatch(DEVICE_OBJECT *DeviceObject, IRP *Irp)
{
	//DbgBreakPoint();
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(Irp);
	//Irp->AssociatedIrp.SystemBuffer;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	int Length = ioStack->Parameters.Write.Length;
	if (Length == sizeof(CommPackage) && gCommCallback)
	{
		PCommPackage package = Irp->AssociatedIrp.SystemBuffer;
		if(package->id == _COMM_ID)	// 校验通信id
		if(MmIsAddressValid(package)) status = gCommCallback(package);
	}

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, 0);
	return status;
}


VOID DriverDestoryComm(PDRIVER_OBJECT  pDriver)
{
	UNICODE_STRING symName = { 0 };
	RtlInitUnicodeString(&symName, SYM_NAME);
	IoDeleteSymbolicLink(&symName);
	if(pDriver->DeviceObject) IoDeleteDevice(pDriver->DeviceObject);
}

NTSTATUS DriverRegisterComm(PDRIVER_OBJECT  pDriver, CommCallback  callback)
{
	

	UNICODE_STRING unName = { 0 };
	RtlInitUnicodeString(&unName, DEVICE_NAME);

	UNICODE_STRING symName = { 0 };
	RtlInitUnicodeString(&symName, SYM_NAME);

	PDEVICE_OBJECT pDevice = NULL;

	NTSTATUS status = IoCreateDevice(pDriver, 0, &unName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDevice);

	if (!NT_SUCCESS(status))
	{
		KdPrintEx((77, 0, "[db]:%x\r\n", status));
		//DbgPrintEx(77, 0, "");
		return status;
	}

	status = IoCreateSymbolicLink(&symName, &unName);

	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevice);
		KdPrintEx((77, 0, "[db]:%x\r\n", status));
		return status;
	}

	pDevice->Flags &= ~DO_DEVICE_INITIALIZING;
	pDevice->Flags |= DO_BUFFERED_IO;

	pDriver->MajorFunction[IRP_MJ_CREATE] = DefDispatch;
	pDriver->MajorFunction[IRP_MJ_CLOSE] = DefDispatch;
	pDriver->MajorFunction[IRP_MJ_WRITE] = WriteDispatch;
	
	if (NT_SUCCESS(status))
	{
		gCommCallback = callback;
	}

	return status;
}