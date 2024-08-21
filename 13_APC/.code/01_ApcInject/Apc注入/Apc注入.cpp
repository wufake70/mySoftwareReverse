// Apc注入.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>

DWORD EnumThreadInProcess(DWORD pid);
VOID Shellcode();
VOID End();

int main()
{
	DWORD pid = 0;
	DWORD tid = 0;
	printf("input target process: ");
	scanf_s("%d", &pid);
	printf("\n");
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		
	if (!hProcess)
	{
		printf("进程打开失败\r\n");
		return -1;
	}
	EnumThreadInProcess(pid);
	printf("input target thread: ");
	scanf_s("%d", &tid);
	printf("\n");
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, tid);

	if (!hThread)
	{
		printf("打开线程失败\r\n");
		return -1;
	}
	
	//LoadlibraryA
	HMODULE hModule = GetModuleHandleA("kernel32.dll");
	PVOID LoadlibraryAptr = (PVOID)GetProcAddress(hModule, "LoadLibraryA");

	printf("LoadlibraryAptr = %x\r\n", LoadlibraryAptr);
	system("pause>nul");
	
	PUCHAR DllFullPath = (PUCHAR)VirtualAllocEx(hProcess, NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
	if (!DllFullPath)
	{
		printf("申请内存失败\r\n");
		return -1;
	}

	char bufPath[0x100] = { 0 };
	printf("input dll full path: ");
	scanf_s("%s", bufPath,0x100);
	printf("\n");
	system("pause>nul");

	SIZE_T proc = NULL;
	if (!WriteProcessMemory(hProcess, DllFullPath, bufPath, strlen(bufPath) + 1, &proc))
	{
		printf("写入内存失败\r\n");
		return -1;
	}
	
	printf("DllFullPath = %x\r\n", DllFullPath);

	QueueUserAPC((PAPCFUNC)LoadlibraryAptr, hThread, (ULONG_PTR)DllFullPath);
	system("pause");
    return 0;
}

DWORD EnumThreadInProcess(DWORD pid) {
	HANDLE processId = 0;
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Failed to create thread snapshot. Error code: " << GetLastError() << std::endl;
		return -1;
	}

	THREADENTRY32 te32;
	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32))
	{
		std::cerr << "Error getting first thread. Error code: " << GetLastError() << std::endl;
		CloseHandle(hThreadSnap);
		return -1;
	}

	do
	{
		if (te32.th32OwnerProcessID == pid)
		{
			std::cout << "Thread ID: " << te32.th32ThreadID << std::endl;
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return 0;
}

VOID Shellcode() {
	

}
VOID End() {

}