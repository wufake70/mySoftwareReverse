#include <ntifs.h>
#include <ntstrsafe.h>
#include <ntimage.h>
#include <intrin.h>

// �ر�д����
ULONG wpOff()
{
	ULONG cr0 = __readcr0();
	// �ر��ж�
	_disable();
	// cr0��wpλ���㣬���ɹر�д����
	__writecr0(cr0 & (~0x10000));
	return cr0;
}

// ����д����
VOID wpOn(ULONG value)
{
	__writecr0(value);
	_enable();
}

PUCHAR  GetImageFileOep(PUCHAR imgBase);

VOID LoadImageCallBack(
	_In_opt_ PUNICODE_STRING FullImageName,// ģ���ȫ·��
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
	// �Ƴ��ص�����
	PsRemoveLoadImageNotifyRoutine(LoadImageCallBack);
	KdPrintEx((77, 0, "DriverUnload\n"));
}


VOID LoadImageCallBack(
	_In_opt_ PUNICODE_STRING FullImageName,
	_In_ HANDLE ProcessId,                // pid into which image is being mapped
	_In_ PIMAGE_INFO ImageInfo
) {
	if (ImageInfo->SystemModeImage == 1) {	// sys
		KdPrintEx((77, 0, "[dbg] ��������, %wZ\n", FullImageName));
		KdPrintEx((77,0,"OEP:%x\n",GetImageFileOep(ImageInfo->ImageBase)));

		// ��ڵ�ֱ�� ret
		ULONG_PTR value = wpOff();	// �ر�д����
		*(PULONG*)GetImageFileOep(ImageInfo->ImageBase)= 0x900008C2;
		wpOn(value);

	}
	else
	{
		//KdPrintEx((77, 0, "[dbg] dll����, %wZ\n", FullImageName));

	}
}

  PUCHAR GetImageFileOep(PUCHAR imgBase) {
	if (imgBase == NULL) return NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = NULL;
	
	pDosHeader = imgBase;
	if (pDosHeader->e_magic != 0x5a4d) return NULL;
	pNtHeader = imgBase+pDosHeader->e_lfanew;
	if (pNtHeader->Signature != 0x4550) return NULL;

	pOptionalHeader = &pNtHeader->OptionalHeader;
	return imgBase+pOptionalHeader->AddressOfEntryPoint;

}