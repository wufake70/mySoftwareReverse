#include <windows.h>
#include <iostream>
#include "search.h"

PCHAR g_OriginStart = NULL;
PCHAR g_CodeData = NULL;
SIZE_T g_CodeSize = 0;

PVOID SaveLdrInitializeThunk() {
	FARPROC LdrInitializeThunkAddr = GetProcAddress(LoadLibraryA("ntdll"), "LdrInitializeThunk");
	ULONG_PTR start = (ULONG_PTR)LdrInitializeThunkAddr;
	ULONG_PTR end = searchOtherCode((ULONG_PTR)LdrInitializeThunkAddr,
		(ULONG_PTR)LdrInitializeThunkAddr + 0x100, (PCHAR)"6A01FF7508e8****50e8****",
		15);
	g_OriginStart = (PCHAR)start;
	PCHAR CodeData = (PCHAR)VirtualAlloc(NULL,end-start, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	memset(CodeData, 0, end - start);
	g_CodeSize = end - start;
	memcpy(CodeData,(PCHAR)start, end - start);
	return CodeData;
}

BOOLEAN CheckCode(PCHAR code1,PCHAR code2,SIZE_T size) {
	int i = 0;
	while (i < size) {
		if (code1[i] != code2[i++]) return FALSE;
	}
	return TRUE;
}

DWORD WINAPI ThreadProc(LPVOID parameter) {
	while (1) {
		if (!CheckCode(g_CodeData, g_OriginStart, g_CodeSize)) {
			std::cout << "Exit Process\n" << std::endl;
			//ExitProcess(-1);

			// 还原代码
			DWORD OldProtect = 0;
			VirtualProtect(g_OriginStart,g_CodeSize,PAGE_EXECUTE_READWRITE,&OldProtect);
			memcpy(g_OriginStart, g_CodeData, g_CodeSize);
			VirtualProtect(g_OriginStart, g_CodeSize, OldProtect, &OldProtect);
		}
	}
}
VOID HookInt3() {
	FARPROC DbgBreakPointAddr = GetProcAddress(LoadLibraryA("ntdll"), "DbgBreakPoint");
	DWORD OldProtect = 0;
	VirtualProtect(DbgBreakPointAddr, 1, PAGE_EXECUTE_READWRITE, &OldProtect);
	*(PCHAR)DbgBreakPointAddr = 0xC3;
	VirtualProtect(DbgBreakPointAddr, 1, OldProtect, &OldProtect);
}
int main()
{
	// 保存LdrInitializeThunk函数
	g_CodeData = (PCHAR)SaveLdrInitializeThunk();
	CreateThread(NULL, 0, ThreadProc, NULL, 0, 0);
	HookInt3();
	while (1) {
		std::cout << "Hello World!\n";
		Sleep(2000);
	}
}

