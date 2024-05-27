
#include <stdio.h>
#include <Windows.h>
#include <winternl.h>


//#pragma comment(linker,"/entry:EntryMain")

// 获取kernel32基址（兼容x86与x64）
HMODULE GetKernel32Address();


//int EntryMain()
int main()
{
    printf("%I64x",GetKernel32Address());
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





