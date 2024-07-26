#include "tools.h"


// 查询内核模块得到指定模块的基址、大小，从而确定整个模块文件
ULONG_PTR QueryModule(PUCHAR moduleName, ULONG_PTR * moduleSize)
{
	if (moduleName == NULL) return 0;

	RTL_PROCESS_MODULES rtlMoudles = { 0 };
	PRTL_PROCESS_MODULES SystemMoudles = &rtlMoudles;
	BOOLEAN isAllocate = FALSE;
	//测量长度
	ULONG * retLen = 0;
	// 查询系统模块信息
	NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, SystemMoudles, sizeof(RTL_PROCESS_MODULES), &retLen);

	//分配实际长度内存
	if (status == STATUS_INFO_LENGTH_MISMATCH)
	{
		SystemMoudles = ExAllocatePool(PagedPool, retLen + sizeof(RTL_PROCESS_MODULES));
		if (!SystemMoudles) return 0;

		memset(SystemMoudles, 0, retLen + sizeof(RTL_PROCESS_MODULES));

		status = ZwQuerySystemInformation(SystemModuleInformation, SystemMoudles, retLen + sizeof(RTL_PROCESS_MODULES), &retLen);
	
		if (!NT_SUCCESS(status))
		{
			ExFreePool(SystemMoudles);
			return 0;
		}

		isAllocate = TRUE;
	}

	PUCHAR kernelModuleName = NULL;
	ULONG_PTR moudleBase = 0;

	do 
	{
	
		// 内核中第一个模块就是内核文件
		// win7 x86中有两个内核文件，ntoskrnl.exe、ntkrnlpa.exe,
		// 编译出来的sys 导入了 ntoskrnl.exe,但实际加载了ntkrnlpa.exe
		if (_stricmp(moduleName, "ntoskrnl.exe") == 0 || _stricmp(moduleName, "ntkrnlpa.exe") == 0)
		{
			PRTL_PROCESS_MODULE_INFORMATION moudleInfo = &SystemMoudles->Modules[0];
			moudleBase = moudleInfo->ImageBase;
			if (moduleSize) *moduleSize = moudleInfo->ImageSize;
			
			break;
		}


		kernelModuleName = ExAllocatePool(PagedPool, strlen(moduleName) + 1);
		memset(kernelModuleName, 0, strlen(moduleName) + 1);
		memcpy(kernelModuleName, moduleName, strlen(moduleName));
		_strupr(kernelModuleName);


		for (int i = 0; i < SystemMoudles->NumberOfModules; i++)
		{
			PRTL_PROCESS_MODULE_INFORMATION moudleInfo = &SystemMoudles->Modules[i];
		
			PUCHAR pathName = _strupr(moudleInfo->FullPathName);
			DbgPrintEx(77, 0, "baseName = %s,fullPath = %s\r\n", 
				moudleInfo->FullPathName + moudleInfo->OffsetToFileName, moudleInfo->FullPathName);

			
			if (strstr(pathName, kernelModuleName))
			{
				moudleBase = moudleInfo->ImageBase;
				if (moduleSize) *moduleSize = moudleInfo->ImageSize;
				break;
			}

		}

	} while (0);
	

	if (kernelModuleName)
	{
		ExFreePool(kernelModuleName);
	}

	if (isAllocate)
	{
		ExFreePool(SystemMoudles);
	}

	return moudleBase;
}

BOOLEAN QueryAllModule()
{
	RTL_PROCESS_MODULES rtlMoudles = { 0 };
	PRTL_PROCESS_MODULES SystemMoudles = &rtlMoudles;
	BOOLEAN isAllocate = FALSE;
	//测量长度
	ULONG * retLen = 0;
	NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, SystemMoudles, sizeof(RTL_PROCESS_MODULES), &retLen);

	//分配实际长度内存
	if (status == STATUS_INFO_LENGTH_MISMATCH)
	{
		SystemMoudles = ExAllocatePool(PagedPool, retLen + sizeof(RTL_PROCESS_MODULES));
		if (!SystemMoudles) return 0;

		memset(SystemMoudles, 0, retLen + sizeof(RTL_PROCESS_MODULES));

		status = ZwQuerySystemInformation(SystemModuleInformation, SystemMoudles, retLen + sizeof(RTL_PROCESS_MODULES), &retLen);

		if (!NT_SUCCESS(status))
		{
			ExFreePool(SystemMoudles);
			return FALSE;
		}

		isAllocate = TRUE;
	}

	PUCHAR kernelModuleName = NULL;
	ULONG_PTR moudleBase = 0;

	for (int i = 0; i < SystemMoudles->NumberOfModules; i++)
	{
		PRTL_PROCESS_MODULE_INFORMATION moudleInfo = &SystemMoudles->Modules[i];
		PUCHAR pathName = _strupr(moudleInfo->FullPathName);
		KdPrintEx((77, 0, "[dbg]:%d\tbaseName=%s\tfullPath=%s\r\n",i,
			moudleInfo->FullPathName + moudleInfo->OffsetToFileName, moudleInfo->FullPathName));

	}
	   
	if (kernelModuleName)
	{
		ExFreePool(kernelModuleName);
	}

	if (isAllocate)
	{
		ExFreePool(SystemMoudles);
	}

	return TRUE;
}