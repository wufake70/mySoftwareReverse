#include <ntifs.h>
#include "tools.h"
#include "Loader.h"
#include "dll.h"

// ����������

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{

}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	// sysData:���ܺ��sys�ļ�
	ULONG dwImageSize = sizeof(sysData);
	unsigned char * pMemory = (unsigned char *)ExAllocatePool(NonPagedPool,dwImageSize);
	memcpy(pMemory, sysData, dwImageSize);
	// ����
	for (ULONG i = 0; i < dwImageSize; i++)
	{
		pMemory[i] ^= 0xDE;
		pMemory[i] ^= 0x73;
	}
	LoadDriver(pMemory);	// ���ز�ִ��
	ExFreePool(pMemory);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_UNSUCCESSFUL;		// ִ�����ж��
}