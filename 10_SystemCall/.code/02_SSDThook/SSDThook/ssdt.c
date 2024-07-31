#include "ssdt.h"
#include "tools.h"
#include <intrin.h>

PUCHAR gMapNtdll = NULL;

//导出服务表
extern PSsdtTable KeServiceDescriptorTable;

// 关闭写保护
ULONG wpOff()
{
	ULONG cr0 = __readcr0();
	// 关闭中断
	_disable(); 
	// cr0的wp位置零，即可关闭写保护
	__writecr0(cr0 & (~0x10000));
	return cr0;
}

// 开启写保护
VOID wpOn(ULONG value)
{
	__writecr0(value);
	_enable();
}

BOOLEAN SsdtInit()
{
	if (gMapNtdll) return TRUE;
	PWCH path = GetSystemRootNtdllPath();
	gMapNtdll = MapOfViewFile(path);
	ExFreePool(path);
	return TRUE;
}

VOID SsdtDestory()
{
	if (!gMapNtdll) return;
	UmMapOfViewFile(gMapNtdll);
	gMapNtdll = NULL;
}

PSsdtTable SsdtGet()
{
	// ssdt、sssdt
	return (PSsdtTable)((PUCHAR)KeServiceDescriptorTable + 0x40);
}

// 获取函数的服务号(4字节数据)
ULONG SsdtGetFunctionIndex(char * funName)
{
	PUCHAR func = (PUCHAR)ExportTableFuncByName(gMapNtdll, funName);
	if (!func) return -1;
	return *(PULONG)(func + 1);
}

ULONG_PTR SsdtSetHook(char * funName, ULONG_PTR newFunction)
{
	PSsdtTable ssdtTable = SsdtGet();

	ULONG index = SsdtGetFunctionIndex(funName);

	if (index == -1) return 0;

	ULONG function = ssdtTable->ssdt.funcTable[index];

	ULONG _cr0 = wpOff();

	ssdtTable->ssdt.funcTable[index] = newFunction;

	wpOn(_cr0);

	return function;

}

