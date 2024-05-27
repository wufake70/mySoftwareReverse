#include "shellcode.h"
#include "obf.h"


/*
* 
确保 shellcode中函数生成顺序
*/

// shellcode 开始标志
int ShellCodeMain()
{
    PGetProcAddress pGetProcAddress = 0;
    PLoadLibraryA pLoadLibraryA = 0;
    if (Init(&pGetProcAddress, &pLoadLibraryA) == FALSE) return 0;
    /*
        vs 2022 字符数组动态赋值，'L','o','a','d' 会一个一个赋值
        LLVM(clang-cl),会优化 'L','o',...,'A'到 其他区段(section)
    */
    //CHAR szUser32[] = {'U','s','e','r','3','2','.','d','l','l',0};
    //CHAR szMessageBoxA[] = { 'M','e','s','s','a','g','e','B','o','x','A',0};
    //CHAR szDbgInfo[] = { 0x66,0x75,0x63,0x6b,0x20,0x79,0x6f,0x75,0};
    CHAR* szUser32 = A("User32.dll");
    CHAR* szMessageBoxA = A("MessageBoxA");
    CHAR* szDbgInfo = A("fuck you");

    HMODULE hUser32dll = 0;
    PMessageBoxA pMessageBoxA = 0;

    
     // 加载 User32.dll
    hUser32dll = pLoadLibraryA((LPSTR)szUser32);
    if (hUser32dll == 0) return 0;
    pMessageBoxA = (PMessageBoxA)pGetProcAddress(hUser32dll, (LPSTR)szMessageBoxA);
    if (pMessageBoxA == 0) return 0;

    pMessageBoxA(0, szDbgInfo, 0, 0);
	return 0;
}









