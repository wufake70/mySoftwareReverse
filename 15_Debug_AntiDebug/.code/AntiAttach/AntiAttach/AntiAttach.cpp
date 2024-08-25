#include <iostream>
#include <Windows.h>
#include <intrin.h>
#include "Search.h"


typedef VOID(WINAPI *FunPtrDebugBreak)(VOID);

VOID MyDebugBreak(VOID) {

	MessageBox(NULL, TEXT("no int3"), TEXT("tips"), 0); 
	ExitProcess(-1);

	// original code
	//__asm {
	//	int 3;
	//	ret;
	//}
}
int end(int a1,int a2) {
	return a1 + a2;
};

// 去除跳板 jmp xxxx
ULONG_PTR GetRealFuncAddr(ULONG_PTR jmpBoard) {
	if (*(PUCHAR)jmpBoard == 0xe9) {

		return jmpBoard = (ULONG_PTR)jmpBoard + *(PULONG)((PUCHAR)jmpBoard + 1) + 5;
	}
	return jmpBoard;
}

int main()
{
	std::cout << std::hex << MyDebugBreak << std::endl;
	FARPROC SearchStartAddr = (FARPROC)(GetProcAddress(LoadLibraryA("ntdll.dll"), "DbgUiRemoteBreakin"));
#ifdef _WIN64
	SearchStartAddr = (FARPROC)searchOtherCode((ULONG_PTR)SearchStartAddr,
		(ULONG_PTR)SearchStartAddr + 0x100,
		(PCHAR)"FFD00F1F00E8****EB00", 6);

#else
	SearchStartAddr = (FARPROC)searchOtherCode((ULONG_PTR)SearchStartAddr,
		(ULONG_PTR)SearchStartAddr + 0x100,
		(PCHAR)"8365FC00E8****EB07", 5);
#endif // _WIN64


	// 修改权限为可写
	DWORD oldProtect;
	if (VirtualProtect(SearchStartAddr, sizeof(ULONG), PAGE_READWRITE, &oldProtect)) {
		// 现在可以修改只读地址的内容了
		ULONG_PTR MyDebugBreakAddr = GetRealFuncAddr((ULONG_PTR)MyDebugBreak);
		PUCHAR ShellCode = NULL; 
		// x64下两个地址偏移大于 4Gb时，e8 e9 调用无效了
		if (MyDebugBreakAddr - (ULONG_PTR)SearchStartAddr - 4 > 0xffffffff) {
			ULONG i = 0;
			do {
				i++;
				if ((ULONG_PTR)SearchStartAddr + i * 0x1000 - (ULONG_PTR)SearchStartAddr - 4 > 0xFFFFFFFF) break;
				ShellCode = (PUCHAR)VirtualAlloc((PUCHAR)SearchStartAddr + i * 0x1000, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			} while (!ShellCode);
			// call 绝对地址 (共16字节)
			//*(PSHORT)ShellCode = 0x15ff;
			//*(PULONG)&ShellCode[2] = 0x02;
			//*(PSHORT)&ShellCode[6] = 0x8eb;
			//*(PULONG_PTR)&ShellCode[8] = MyDebugBreakAddr;

			// jmp 绝对地址 (共14字节)
			*(PSHORT)ShellCode = 0x25ff;
			*(PULONG)&ShellCode[2] = 0;
			*(PULONG_PTR)&ShellCode[6] = MyDebugBreakAddr;

		}
#ifdef _WIN64
		*(PULONG)SearchStartAddr = (ULONG_PTR)ShellCode - (ULONG_PTR)SearchStartAddr -4;
#else
		*(PULONG)SearchStartAddr = (ULONG_PTR)MyDebugBreakAddr - (ULONG_PTR)SearchStartAddr -4;
#endif
		
		// 恢复原来的权限
		VirtualProtect(SearchStartAddr, sizeof(ULONG), oldProtect, &oldProtect);

		std::cout 
			<< "成功修改只读地址的内容为: " 
			<< std::hex 
			<< *(PULONG)SearchStartAddr
			<< std::endl;
	}
	else {
		std::cerr << "修改内存权限失败" << std::endl;
	}


	while (1) {
		std::cout << "Hello World!\n";
		Sleep(1500);
	}

}