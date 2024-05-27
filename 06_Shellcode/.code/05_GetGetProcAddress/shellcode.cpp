
#include <stdio.h>
#include <Windows.h>
#include <winternl.h>


#pragma comment(linker,"/entry:EntryMain")

// 获取kernel32基址（兼容x86与x64）
HMODULE GetKernel32Address();

// 获取GetPorcAddress函数地址
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
(WINAPI *PMessageBoxA)(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCSTR lpText,
    _In_opt_ LPCSTR lpCaption,
    _In_ UINT uType);

int EntryMain()
//int main()
{
    /*
        vs 2022 字符数组动态赋值，'L','o','a','d' 会一个一个赋值
        LLVM(clang-cl),会优化 'L','o',...,'A'到 其他区段(section)
    */
    CHAR szLoadLibraryA[] = {'L','o','a','d','L','i','b','r','a','r','y','A',0};
    CHAR szUser32[] = {'U','s','e','r','3','2','.','d','l','l',0};
    CHAR szMessageBoxA[] = { 'M','e','s','s','a','g','e','B','o','x','A',0};
    CHAR szDbgInfo[] = { 0x66,0x75,0x63,0x6b,0x20,0x79,0x6f,0x75,0};

    HMODULE hKernel32dll = 0;
    HMODULE hUser32dll = 0;
    PGetProcAddress pGetProcAddress = 0;
    PLoadLibraryA pLoadLibraryA = 0;
    PMessageBoxA pMessageBoxA = 0;

    // 获取 kernel32.dll基址
    hKernel32dll = GetKernel32Address();
    if (hKernel32dll == 0) return 0;

    // 获取 GetPorcAddress
    pGetProcAddress = (PGetProcAddress)GetGetPorcAddress(hKernel32dll);
    if (pGetProcAddress == 0) return 0;

    // 获取 LoadLibraryA
    pLoadLibraryA = (PLoadLibraryA)pGetProcAddress(hKernel32dll, (LPSTR)szLoadLibraryA);
    if (pLoadLibraryA == 0) return 0;
    
     // 加载 User32.dll
    hUser32dll = pLoadLibraryA((LPSTR)szUser32);
    if (hUser32dll == 0) return 0;
    pMessageBoxA = (PMessageBoxA)pGetProcAddress(hUser32dll, (LPSTR)szMessageBoxA);
    if (pMessageBoxA == 0) return 0;

    pMessageBoxA(0, szDbgInfo, 0, 0);
	return 0;
}

HMODULE GetKernel32Address()
{
    HMODULE hKernel32 = NULL;

    // 用户保存模块名
    WCHAR wszModuleName[MAX_PATH];

#ifdef _WIN64    // 64位PEB偏移为0x60
    PPEB lpPeb = (PPEB)__readgsqword(0x60);
#else            // 32位PEB偏移为0x30
    PPEB lpPeb = (PPEB)__readfsdword(0x30);
#endif

    PLIST_ENTRY pListHead = &lpPeb->Ldr->InMemoryOrderModuleList;
    PLIST_ENTRY pListData = pListHead->Flink;

    // 遍历所有模块
    while (pListData != pListHead)
    {
        PLDR_DATA_TABLE_ENTRY pLDRData = CONTAINING_RECORD(pListData, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        DWORD dwLen = pLDRData->FullDllName.Length / 2;
        if (dwLen > 12)    // 12 是"kernel32.dll"的长度，获取到的完整路径肯定要比模块名长
        {
            // 从获取到的模块完整路径中提取模块名
            for (size_t i = 0; i < 12; i++)
            {
                wszModuleName[11 - i] = pLDRData->FullDllName.Buffer[dwLen - 1 - i];
            }

            // 最终要获取的目标模块名（"kernel32.dll"），逐个字节比较，包含大小写。
            if ((wszModuleName[0] == 'k' || wszModuleName[0] == 'K') &&
                (wszModuleName[1] == 'e' || wszModuleName[1] == 'E') &&
                (wszModuleName[2] == 'r' || wszModuleName[2] == 'R') &&
                (wszModuleName[3] == 'n' || wszModuleName[3] == 'N') &&
                (wszModuleName[4] == 'e' || wszModuleName[4] == 'E') &&
                (wszModuleName[5] == 'l' || wszModuleName[5] == 'L') &&
                (wszModuleName[6] == '3') &&
                (wszModuleName[7] == '2') &&
                (wszModuleName[8] == '.') &&
                (wszModuleName[9] == 'd' || wszModuleName[9] == 'D') &&
                (wszModuleName[10] == 'l' || wszModuleName[10] == 'L') &&
                (wszModuleName[11] == 'l' || wszModuleName[11] == 'L'))
            {
                hKernel32 = (HMODULE)pLDRData->DllBase;
                break;
            }
        }
        pListData = pListData->Flink;
    }
    return hKernel32;
}


FARPROC GetGetPorcAddress(HMODULE hKernel32)
{
    // 保存最终结果
    FARPROC pGetPorcAddress = NULL;

    if (!hKernel32)
    {
        return NULL;
    }

    PIMAGE_DOS_HEADER lpDosHeader = (PIMAGE_DOS_HEADER)hKernel32;
    PIMAGE_NT_HEADERS lpNTHeader = (PIMAGE_NT_HEADERS)((unsigned char*)hKernel32 + lpDosHeader->e_lfanew);

    // 模块有效性验证
    if (!lpNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
    {
        return NULL;
    }
    if (!lpNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)
    {
        return NULL;
    }

    // 通过导出表中的导出函数名，定位"GetProcAddress"的位置
    PIMAGE_EXPORT_DIRECTORY lpExports = (PIMAGE_EXPORT_DIRECTORY)((unsigned char*)hKernel32 + lpNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    PDWORD lpdwFunName = (PDWORD)((unsigned char*)hKernel32 + lpExports->AddressOfNames);
    PWORD lpdwOrd = (PWORD)((unsigned char*)hKernel32 + lpExports->AddressOfNameOrdinals);
    PDWORD lpdwFunAddr = (PDWORD)((unsigned char*)hKernel32 + lpExports->AddressOfFunctions);

    for (DWORD dwLoop = 0; dwLoop <= lpExports->NumberOfNames - 1; dwLoop++)
    {
        char* pFunName = (char*)(lpdwFunName[dwLoop] + (unsigned char*)hKernel32);
        // 比较函数名
        if (
            pFunName[0] == 'G' &&
            pFunName[1] == 'e' &&
            pFunName[2] == 't' &&
            pFunName[3] == 'P' &&
            pFunName[4] == 'r' &&
            pFunName[5] == 'o' &&
            pFunName[6] == 'c' &&
            pFunName[7] == 'A' &&
            pFunName[8] == 'd' &&
            pFunName[9] == 'd' &&
            pFunName[10] == 'r' &&
            pFunName[11] == 'e' &&
            pFunName[12] == 's' &&
            pFunName[13] == 's'
            )
        {
            pGetPorcAddress = (FARPROC)(lpdwFunAddr[lpdwOrd[dwLoop]] + (unsigned char*)hKernel32);
            break;
        }
    }
    return pGetPorcAddress;
}




