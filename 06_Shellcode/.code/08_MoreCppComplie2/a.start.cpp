#include "a.start.h"
#include "shellcode.h"
#include "z.end.h"

#pragma comment(linker,"/entry:EntryMain")


int EntryMain()
{
    /*
    ���ڷ�������Ч����Ϊ���԰棬������������ַǰ������һ��jmpָ��������
    */
    HANDLE hFile = CreateFileA("./shellcode.bin",
        GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) return 1;
    DWORD dwWritenNums = 0;
#ifdef _WIN64
    WriteFile(hFile, Start, (DWORD)((ULONG64)End - (ULONG64)Start), &dwWritenNums, NULL);
#else
    WriteFile(hFile, Start, (DWORD)End - (DWORD)Start, &dwWritenNums, NULL);
#endif // _WIN64

    CloseHandle(hFile);

    return 0;
}

int Start()
{
    ShellCodeMain();
    return 0;
}