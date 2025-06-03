// VEHHook.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include "VHook.h"

void WINAPI  HookOpenProcess(PCONTEXT_INFO contextInfo)
{
	printf("this is HookOpenProcess\r\n");
	return;
}
 
int main()
{
	HMODULE hModule = GetModuleHandleA("kernel32.dll");

	ULONG OpenProcessPtr = (ULONG)GetProcAddress(hModule, "OpenProcess");
	ULONG LoadLibraryWPtr = (ULONG)GetProcAddress(hModule, "LoadLibraryW");
 	if (InitVEHook())
	{
		AddHook((ULONG)OpenProcessPtr, HookOpenProcess);
		AddHook((ULONG)OpenProcessPtr, HookOpenProcess);
		AddHook((ULONG)LoadLibraryW, HookOpenProcess);
	}
	OpenProcess(NULL, NULL, NULL);
	LoadLibraryW(L"");
	DestroyVEHook();
	system("pause");
	
    return 0;
}

