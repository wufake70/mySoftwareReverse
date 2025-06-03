#pragma once
#include <Windows.h>

typedef struct _CONTEXT_INFO
{

	ULONG_PTR eflag;
	ULONG_PTR edi;
	ULONG_PTR esi;
	ULONG_PTR ebp;
	ULONG_PTR esp;
	ULONG_PTR ebx;
	ULONG_PTR edx;
	ULONG_PTR ecx;
	ULONG_PTR eax;

}CONTEXT_INFO, *PCONTEXT_INFO;

bool InitVEHook();
void DestroyVEHook();

typedef void (WINAPI * HookCallback)(PCONTEXT_INFO contextInfo);

bool AddHook(ULONG_PTR HookAddr, HookCallback newAddr);