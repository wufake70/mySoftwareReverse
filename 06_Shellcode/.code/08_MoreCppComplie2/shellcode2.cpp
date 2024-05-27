#include "shellcode.h"



HMODULE GetKernel32Address()
{
    HMODULE hKernel32 = NULL;

    // �û�����ģ����
    WCHAR wszModuleName[MAX_PATH];

#ifdef _WIN64    // 64λPEBƫ��Ϊ0x60
    PPEB lpPeb = (PPEB)__readgsqword(0x60);
#else            // 32λPEBƫ��Ϊ0x30
    PPEB lpPeb = (PPEB)__readfsdword(0x30);
#endif

    PLIST_ENTRY pListHead = &lpPeb->Ldr->InMemoryOrderModuleList;
    PLIST_ENTRY pListData = pListHead->Flink;

    // ��������ģ��
    while (pListData != pListHead)
    {
        PLDR_DATA_TABLE_ENTRY pLDRData = CONTAINING_RECORD(pListData, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        DWORD dwLen = pLDRData->FullDllName.Length / 2;
        if (dwLen > 12)    // 12 ��"kernel32.dll"�ĳ��ȣ���ȡ��������·���϶�Ҫ��ģ������
        {
            // �ӻ�ȡ����ģ������·������ȡģ����
            for (size_t i = 0; i < 12; i++)
            {
                wszModuleName[11 - i] = pLDRData->FullDllName.Buffer[dwLen - 1 - i];
            }

            // ����Ҫ��ȡ��Ŀ��ģ������"kernel32.dll"��������ֽڱȽϣ�������Сд��
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
    // �������ս��
    FARPROC pGetPorcAddress = NULL;

    if (!hKernel32)
    {
        return NULL;
    }

    PIMAGE_DOS_HEADER lpDosHeader = (PIMAGE_DOS_HEADER)hKernel32;
    PIMAGE_NT_HEADERS lpNTHeader = (PIMAGE_NT_HEADERS)((unsigned char*)hKernel32 + lpDosHeader->e_lfanew);

    // ģ����Ч����֤
    if (!lpNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
    {
        return NULL;
    }
    if (!lpNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)
    {
        return NULL;
    }

    // ͨ���������еĵ�������������λ"GetProcAddress"��λ��
    PIMAGE_EXPORT_DIRECTORY lpExports = (PIMAGE_EXPORT_DIRECTORY)((unsigned char*)hKernel32 + lpNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    PDWORD lpdwFunName = (PDWORD)((unsigned char*)hKernel32 + lpExports->AddressOfNames);
    PWORD lpdwOrd = (PWORD)((unsigned char*)hKernel32 + lpExports->AddressOfNameOrdinals);
    PDWORD lpdwFunAddr = (PDWORD)((unsigned char*)hKernel32 + lpExports->AddressOfFunctions);

    for (DWORD dwLoop = 0; dwLoop <= lpExports->NumberOfNames - 1; dwLoop++)
    {
        char* pFunName = (char*)(lpdwFunName[dwLoop] + (unsigned char*)hKernel32);
        // �ȽϺ�����
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