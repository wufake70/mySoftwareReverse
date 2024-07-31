#pragma once
#include <ntifs.h>

PWCH GetSystemRootNtdllPath();

PUCHAR MapOfViewFile(PWCH path);

VOID UmMapOfViewFile(PVOID mapBase);

ULONG64 ExportTableFuncByName(char * pData, char * funcName);