#include <ntifs.h>
#include <ntstrsafe.h>

#define DEVICE_NAME L"\\device\\mydevic"
#define SYMBL_LINK_NAME L"\\??\\mydevice"

// 自定义设备控制码
#define CODE_READ CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define CODE_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN,0x900,METHOD_BUFFERED,FILE_ANY_ACCESS)

// 设备对象的各种回调函数
NTSTATUS CreateDeviceCallBack(
	_In_ struct _DEVICE_OBJECT *DeviceObject,
	_Inout_ struct _IRP *Irp
);
NTSTATUS CloseDeviceCallBack(
	_In_ struct _DEVICE_OBJECT *DeviceObject,
	_Inout_ struct _IRP *Irp
);
NTSTATUS ControlDeviceCallBack(
	_In_ struct _DEVICE_OBJECT *DeviceObject,
	_Inout_ struct _IRP *Irp
);

VOID DriverUnload(_In_ struct _DRIVER_OBJECT *pDriver)
{
	UNICODE_STRING SymbolLinkName = { 0 };
	RtlInitUnicodeString(&SymbolLinkName,SYMBL_LINK_NAME);
	IoDeleteSymbolicLink(&SymbolLinkName);
	IoDeleteDevice(pDriver->DeviceObject);
	DbgPrint("DriverUnload\n");
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	UNICODE_STRING DeviceName = { 0 };
	UNICODE_STRING SymbolLinkName = { 0 };
	PDEVICE_OBJECT pDevice = { 0 };
	NTSTATUS status;

	pDriver->DriverUnload = DriverUnload;	
	RtlInitUnicodeString(&DeviceName, DEVICE_NAME);
	RtlInitUnicodeString(&SymbolLinkName, SYMBL_LINK_NAME);

	// 创建设备对象(给驱动对象绑定设备对象)
	/*
		NTSTATUS IoCreateDevice(
		PDRIVER_OBJECT DriverObject,           // 驱动程序对象
		ULONG         DeviceExtensionSize,     // 设备扩展大小
		PUNICODE_STRING DeviceName,            // 设备名称
		DEVICE_TYPE   DeviceType,              // 设备类型
		ULONG         DeviceCharacteristics,   // 设备特性
		BOOLEAN       Exclusive,               // 是否独占访问
		PDEVICE_OBJECT *DeviceObject           // 传出的设备对象指针
);
	*/
	status = IoCreateDevice(pDriver,0,&DeviceName,FILE_DEVICE_UNKNOWN,FILE_DEVICE_SECURE_OPEN,TRUE,&pDevice);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(pDevice);
		KdPrintEx((77,0,"make device object fail."));
		return status;
	}
	// 数据传输类型: DO_BUFFERED_IO 缓冲区读写，复制一份。
	//				 DO_DIRECT_IO 直接读写，共享一个物理页
	pDevice->Flags |= DO_BUFFERED_IO;

	// R3也无法访问设备对象，需要创建符号链接
	status = IoCreateSymbolicLink(&SymbolLinkName,&DeviceName);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(pDevice);
		KdPrintEx((77, 0, "make symbol link fail."));
		return status;
	}

	// 功能函数
	pDriver->MajorFunction[IRP_MJ_CREATE]=CreateDeviceCallBack;
	pDriver->MajorFunction[IRP_MJ_CLOSE]=CloseDeviceCallBack;
	pDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL]=ControlDeviceCallBack;
	// pDriver->MajorFunction[IRP_MJ_WRITE] = WriteDispatch;	 // 也可尝试用写派发 	

	return STATUS_SUCCESS;
}


NTSTATUS CreateDeviceCallBack(
	_In_ struct _DEVICE_OBJECT *DeviceObject,
	_Inout_ struct _IRP *Irp
) {
	KdPrintEx((77,0,"make device success.\n"));
	Irp->IoStatus.Status = STATUS_SUCCESS;		// 告诉R3处理的状态
	Irp->IoStatus.Information = STATUS_SUCCESS;	// 告诉R0处理的状态
	IofCompleteRequest(Irp,IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CloseDeviceCallBack(
	_In_ struct _DEVICE_OBJECT *DeviceObject,
	_Inout_ struct _IRP *Irp
) {
	KdPrintEx((77, 0, "close device success.\n"));
	Irp->IoStatus.Status = STATUS_SUCCESS;		// 告诉R3处理的状态
	Irp->IoStatus.Information = STATUS_SUCCESS;	// 告诉R0处理的状态
	IofCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS ControlDeviceCallBack(
	_In_ struct _DEVICE_OBJECT *DeviceObject,
	_Inout_ struct _IRP *Irp
) {
	// 获取Irp协议栈数据
	PIO_STACK_LOCATION pStackLocation = IoGetCurrentIrpStackLocation(Irp);
	// 设备控制码
	ULONG controlCode = pStackLocation->Parameters.DeviceIoControl.IoControlCode;

	PVOID systemBuffer = Irp->AssociatedIrp.SystemBuffer;
	ULONG inputBufLen = pStackLocation->Parameters.DeviceIoControl.InputBufferLength;
	ULONG outputBufLen = pStackLocation->Parameters.DeviceIoControl.OutputBufferLength;

	switch (controlCode) {
	case CODE_READ:
		KdPrintEx((77, 0, "CODE_READ\n"));
		if (outputBufLen >= 0x20) {
			DbgBreakPoint();
			memcpy(systemBuffer, "12345678", sizeof("12345678"));
			Irp->IoStatus.Information = sizeof("12345678");
		}
		else {
			memcpy(systemBuffer, "1", sizeof("1"));
			Irp->IoStatus.Information = 0;
		}
		break;
	case CODE_WRITE:
		KdPrintEx((77, 0, "CODE_WRITE: %s",systemBuffer));
		Irp->IoStatus.Information = 0;
		break;
	}

	Irp->IoStatus.Status = STATUS_SUCCESS;
	IofCompleteRequest(Irp,IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}