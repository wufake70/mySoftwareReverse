
#include <stdio.h>
#include <Windows.h>
#include <winternl.h>

/*
x64不支持嵌入式asm、__declspec(naked)
前者可以修改 "平台工具集"-》LLVM (clang-cl，前提 打开Visual Studio Installer 安装Clang工具集. )

*/

//#pragma comment(linker,"/entry:EntryMain")


#ifdef _WIN64
ULONG64 GetKernel32Address()
{
	ULONG64 result = 0;
	__asm {
		mov rax, gs: [60h] ;	// PEB
		mov rax, [rax + 18h];	// PEB_LDR_DATA，按模块加载顺序
		mov rax, [rax + 10h];	// LIST_ENTRY 主模块（程序本身）
		//mov rax, [rax + 20h];	// LIST_ENTRY 主模块（程序本身）
		mov rax, [rax];			// ntdll
		mov rax, [rax];			// kernel32
		mov rax, [rax + 30h];	// kernel32基址
		//mov rax, [rax + 20h];	// kernel32基址
		mov result, rax;
		//ret;
	}
	return result;
}
#else
__declspec(naked) DWORD GetKernel32Address()
{
	__asm {
		mov eax, fs: [30h] ;			// PEB
		mov eax, [eax + 0ch];		// PEB_LDR_DATA，按模块加载顺序
		mov eax, [eax + 0ch];		// LIST_ENTRY 主模块（程序本身）
		mov eax, [eax];				// ntdll
		mov eax, [eax];				// kernel32
		mov eax, dword ptr[eax + 18h]; // kernel32基址
		ret
	}

}
#endif


// 遍历模块
VOID EnumModuleList();


//int EntryMain()
int main()
{
#ifdef _WIN64
	printf("%I64x", GetKernel32Address());
#else
	printf("%x", GetKernel32Address());
#endif
	printf("\nModule list:\n");
	EnumModuleList();
	system("pause > nul");

	return 0;
}



VOID EnumModuleList()
{
#ifdef _WIN64
	PEB* lpPeb = NULL;
	PEB_LDR_DATA* lpPebLdrData = NULL;
	LIST_ENTRY* lpListEntry = NULL;
	LIST_ENTRY* lpListEntryNext = NULL;
	TCHAR* szName = NULL;

	__asm {
		mov rax, gs: [60h] ;	// PEB
		mov lpPeb, rax;
		mov rax, [rax + 18h];	// PEB_LDR_DATA，按模块加载顺序
		mov lpPebLdrData, rax;
		mov rax, [rax + 10h];	// LIST_ENTRY 主模块（程序本身）
		mov lpListEntry, rax;
		mov rax, [rax + 60h];	// kernel32基址
		mov szName, rax;
	}

	lpListEntryNext = lpListEntry;
	do 	{
		wprintf(L"%s\n", szName);
		__asm {
			mov rax, [lpListEntryNext];
			mov rax, [rax];
			mov lpListEntryNext, rax;
			mov rax, qword ptr[rax + 60h]; // name
			mov szName, rax;

		}

	} while (lpListEntryNext != lpListEntry);


#else
	PEB* lpPeb = NULL;
	PEB_LDR_DATA* lpPebLdrData = NULL;
	LIST_ENTRY* lpListEntry = NULL;
	LIST_ENTRY* lpListEntryNext = NULL;
	TCHAR* szName = NULL;
	__asm {
		mov eax, fs: [30h] ;			// PEB
		mov lpPeb, eax;
		mov eax, [eax + 0ch];		// PEB_LDR_DATA，按模块加载顺序
		mov lpPebLdrData, eax;
		mov eax, [eax + 0ch];		// LIST_ENTRY 主模块
		mov lpListEntry, eax;
		mov eax, dword ptr[eax + 30h]; // name
		mov szName, eax;

	}

	lpListEntryNext = lpListEntry;
	do {
		wprintf(L"%s\n", szName);
		__asm {
			mov eax, [lpListEntryNext];
			mov eax, [eax];
			mov lpListEntryNext, eax;
			mov eax, dword ptr[eax + 30h]; // name
			mov szName, eax;

		}

	} while (lpListEntryNext != lpListEntry);

#endif
}



