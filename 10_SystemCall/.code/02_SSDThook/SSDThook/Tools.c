#include "Tools.h"
#include <ntstrsafe.h>
#include <ntimage.h>


EXTERN_C NTSTATUS MmCreateSection(
	__deref_out PVOID *SectionObject,
	__in ACCESS_MASK DesiredAccess,
	__in_opt POBJECT_ATTRIBUTES ObjectAttributes,
	__in PLARGE_INTEGER InputMaximumSize,
	__in ULONG SectionPageProtection,
	__in ULONG AllocationAttributes,
	__in_opt HANDLE FileHandle,
	__in_opt PFILE_OBJECT FileObject
);

// 获取ntdll.dll路径
PWCH GetSystemRootNtdllPath()
{
	//获取根目录 256
	PWCH  systemPath = ExAllocatePool(PagedPool, PAGE_SIZE);
	memset(systemPath, 0, PAGE_SIZE);
	RtlStringCbPrintfW(systemPath, PAGE_SIZE, L"\\??\\%s\\system32\\ntdll.dll", SharedUserData->NtSystemRoot);
	DbgPrintEx(77, 0, "[db]:%S\r\n", systemPath);
	//ExFreePool(systemPath);
	return systemPath;
}

// 映射指定pe文件
PUCHAR MapOfViewFile(PWCH path)
{

	UNICODE_STRING fileName = {0};
	RtlInitUnicodeString(&fileName, path);
	OBJECT_ATTRIBUTES ObjectAttributes;
	InitializeObjectAttributes(&ObjectAttributes, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	IO_STATUS_BLOCK IoStatusBlock = {0};

	HANDLE hFile = NULL;
	NTSTATUS status = ZwCreateFile(&hFile, GENERIC_READ, &ObjectAttributes, &IoStatusBlock, NULL,
		FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, NULL, NULL);
	
	if (!NT_SUCCESS(status))
	{
		return 0;
		
	}

	
	//创建节区
	PVOID hSection = NULL;
	OBJECT_ATTRIBUTES sectionAddr;
	InitializeObjectAttributes(&sectionAddr, NULL, OBJ_CASE_INSENSITIVE, NULL, NULL);
	LARGE_INTEGER InputMaximumSize = { 0 };
	status = MmCreateSection(&hSection, SECTION_ALL_ACCESS, &sectionAddr, 
		&InputMaximumSize, PAGE_EXECUTE_READWRITE, 0x1000000, hFile,NULL);
	
	if (!NT_SUCCESS(status))
	{
		ZwClose(hFile);
		return 0;
	}

	PVOID mapBase = NULL;
	SIZE_T viewSize = {0};
	status = MmMapViewInSystemSpace(hSection, &mapBase, &viewSize);

	ObDereferenceObject(hSection);
	ZwClose(hFile);

	if (NT_SUCCESS(status))
	{
		return mapBase;
		
	}


	
	return 0;
}

// 取消映射
VOID UmMapOfViewFile(PVOID mapBase)
{
	if (!mapBase) return;
	MmUnmapViewInSystemSpace(mapBase);
}

// 通过名字获取导出函数
ULONG64 ExportTableFuncByName(char * pData, char * funcName)
{
	PIMAGE_DOS_HEADER pHead = (PIMAGE_DOS_HEADER)pData;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pData + pHead->e_lfanew);
	int numberRvaAndSize = pNt->OptionalHeader.NumberOfRvaAndSizes;
	PIMAGE_DATA_DIRECTORY pDir = (PIMAGE_DATA_DIRECTORY)&pNt->OptionalHeader.DataDirectory[0];

	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(pData + pDir->VirtualAddress);

	ULONG64 funcAddr = 0;
	for (int i = 0; i<pExport->NumberOfNames; i++)
	{
		int * funcAddress = pData + pExport->AddressOfFunctions;
		int * names = pData + pExport->AddressOfNames;
		short * fh = pData + pExport->AddressOfNameOrdinals;
		int index = -1;
		char * name = pData + names[i];

		if (strcmp(name, funcName) == 0)
		{
			index = fh[i];
		}



		if (index != -1)
		{
			funcAddr = pData + funcAddress[index];
			break;
		}


	}

	if (!funcAddr)
	{
		KdPrint(("没有找到函数%s\r\n", funcName));

	}
	else
	{
		KdPrint(("找到函数%s addr %p\r\n", funcName, funcAddr));
	}


	return funcAddr;
}