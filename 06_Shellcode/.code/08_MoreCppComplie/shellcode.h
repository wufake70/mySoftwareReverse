#pragma once
#include <Windows.h>
#include <winternl.h>

// ��ȡkernel32��ַ������x86��x64��
HMODULE GetKernel32Address();
// ��ȡGetPorcAddress������ַ
FARPROC GetGetPorcAddress(HMODULE hKernel32);

typedef WINBASEAPI
FARPROC
(WINAPI* PGetProcAddress)
(
    _In_ HMODULE hModule,
    _In_ LPCSTR lpProcName);

typedef WINBASEAPI
_Ret_maybenull_
HMODULE
(WINAPI* PLoadLibraryA)
(
    _In_ LPCSTR lpLibFileName
    );

typedef WINUSERAPI
int
(WINAPI* PMessageBoxA)(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCSTR lpText,
    _In_opt_ LPCSTR lpCaption,
    _In_ UINT uType);

int ShellCodeMain();
