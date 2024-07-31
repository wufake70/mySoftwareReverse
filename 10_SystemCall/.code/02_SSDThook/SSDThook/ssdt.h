#pragma once
#include <ntifs.h>

typedef struct _SsdtItem 
{
	PULONG funcTable;
	ULONG_PTR totalCount;
	ULONG_PTR funcMaxNumber;
	PUCHAR paramTable;
}SsdtItem;

typedef struct _SsdtTable 
{
	SsdtItem ssdt;
	SsdtItem sssdt;
}SsdtTable,*PSsdtTable;



BOOLEAN SsdtInit();

VOID SsdtDestory();

ULONG SsdtGetFunctionIndex(char * funName);

ULONG_PTR SsdtSetHook(char * funName, ULONG_PTR newFunction);