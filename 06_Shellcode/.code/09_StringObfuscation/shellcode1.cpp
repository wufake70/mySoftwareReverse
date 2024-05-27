#include "shellcode.h"
#include "obf.h"

BOOL Init(PGetProcAddress *ppGetProcAddress,
	PLoadLibraryA *ppLoadLibraryA)
{
	//CHAR szLoadLibraryA[] = { 'L','o','a','d','L','i','b','r','a','r','y','A',0 };
	CHAR *szLoadLibraryA = A("LoadLibraryA");

	HMODULE hKernel32dll = 0;
	//PGetProcAddress pGetProcAddress = 0;
	//PLoadLibraryA pLoadLibraryA = 0;

	// 获取 kernel32.dll基址
	hKernel32dll = GetKernel32Address();
	if (hKernel32dll == 0) return FALSE;

	// 获取 GetPorcAddress
	*ppGetProcAddress = (PGetProcAddress)GetGetPorcAddress(hKernel32dll);
	if (ppGetProcAddress == 0) return FALSE;

	// 获取 LoadLibraryA
	*ppLoadLibraryA = (PLoadLibraryA)ppGetProcAddress[0](hKernel32dll, (LPSTR)szLoadLibraryA);
	if (ppLoadLibraryA == 0) return FALSE;

	return TRUE;
}