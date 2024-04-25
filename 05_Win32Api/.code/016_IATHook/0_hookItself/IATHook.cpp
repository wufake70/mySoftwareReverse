// IATHook.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include "LoadPE.h"


WINUSERAPI
int 
WINAPI 
MyMessageBox(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCWSTR lpText,
    _In_opt_ LPCWSTR lpCaption,
    _In_ UINT uType);

BOOL IATHook32(LPVOID lpOldFuncAddr, LPVOID lpNewFuncAddr);
BOOL IATHook64(LPVOID lpOldFuncAddr, LPVOID lpNewFuncAddr);
#ifdef _WIN64  
#define IATHook IATHook64
#else
#define IATHook IATHook32
#endif

// MessageBox函数指针，劫持后在继续运行
typedef int(WINAPI *FunPtr) (   // 调用约定、参数、返回值必须一致
    _In_opt_ HWND hWnd,
    _In_opt_ LPCWSTR lpText,
    _In_opt_ LPCWSTR lpCaption,
    _In_ UINT uType);

FunPtr myMessageBox = (FunPtr)MessageBox;

int main()
{
    
    //FunPtr mb = (FunPtr)MessageBox;
    // 在调用 MessageBox之前修改 IAT对应地址
    IATHook((LPVOID)MessageBox, (LPVOID)MyMessageBox);

	MessageBox(0,TEXT("HHHH"),0,0);

    
}

int 
WINAPI
MyMessageBox(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCWSTR lpText,
    _In_opt_ LPCWSTR lpCaption,
    _In_ UINT uType)
{
    int result = myMessageBox(0,TEXT("Hook Successfully."), TEXT("Modified"), 0);
    return result;
}


BOOL IATHook32(LPVOID lpOldFuncAddr, LPVOID lpNewFuncAddr)
{
    TCHAR szItselfPath[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szItselfPath, MAX_PATH);
    HMODULE hItself = LoadLibrary(szItselfPath);
    DATA_DIRECTORY* pImportTableEntry = NULL;
    IMPORT_DESCRIPTOR* pImportTable = NULL;
    THUNK_DATA32* pIATThunk = NULL;
    DWORD dwOldProtect = 0;

    pImportTableEntry = GetDirectoryEntryPtr((LPBYTE)hItself, 1);
    // 导入表一个dll描述信息
    pImportTable = (IMPORT_DESCRIPTOR*)(pImportTableEntry[0].VirtualAddress + (DWORD)hItself);
    do {
        pIATThunk = (THUNK_DATA32*)(pImportTable[0].FirstThunk + (DWORD)hItself);
        if (!pIATThunk) return FALSE;
        do {
            // 找到MessageBox 地址并修改到指定函数
            if ((DWORD)lpOldFuncAddr == (DWORD)pIATThunk[0].u1.AddressOfData)
            {
                // 修改内存保护属性为可写
                dwOldProtect=0;
                VirtualProtect(&pIATThunk[0].u1.AddressOfData, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);

                // 修改函数地址
                pIATThunk[0].u1.AddressOfData = (DWORD)lpNewFuncAddr;

                // 还原内存保护属性
                VirtualProtect(&pIATThunk[0].u1.AddressOfData, sizeof(DWORD), dwOldProtect, &dwOldProtect);

                return TRUE;
            }
            pIATThunk++;
        } while ((DWORD)pIATThunk[0].u1.AddressOfData);

        pImportTable++;
    } while (pImportTable[0].Characteristics ||
        pImportTable[0].FirstThunk ||
        pImportTable[0].ForwarderChain ||
        pImportTable[0].Name ||
        pImportTable[0].OriginalFirstThunk ||
        pImportTable[0].TimeDateStamp);


    return FALSE;
}


BOOL IATHook64(LPVOID lpOldFuncAddr, LPVOID lpNewFuncAddr)
{
    TCHAR szItselfPath[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szItselfPath, MAX_PATH);
    HMODULE hItself = LoadLibrary(szItselfPath);
    DATA_DIRECTORY* pImportTableEntry = NULL;
    IMPORT_DESCRIPTOR* pImportTable = NULL;
    THUNK_DATA64* pIATThunk = NULL;
    DWORD dwOldProtect = 0;

    pImportTableEntry = GetDirectoryEntryPtr((LPBYTE)hItself, 1);
    // 导入表一个dll描述信息
    pImportTable = (IMPORT_DESCRIPTOR*)(pImportTableEntry[0].VirtualAddress + (ULONG64)hItself);
    do {
        pIATThunk = (THUNK_DATA64*)(pImportTable[0].FirstThunk + (ULONG64)hItself);
        if (!pIATThunk) return FALSE;
        do {
            // 找到MessageBox 地址并修改到指定函数
            if ((ULONG64)lpOldFuncAddr == (ULONG64)pIATThunk[0].u1.AddressOfData)
            {
                // 修改内存保护属性为可写
                dwOldProtect = 0;
                VirtualProtect(&pIATThunk[0].u1.AddressOfData, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);

                // 修改函数地址
                pIATThunk[0].u1.AddressOfData = (ULONG64)lpNewFuncAddr;

                // 还原内存保护属性
                VirtualProtect(&pIATThunk[0].u1.AddressOfData, sizeof(DWORD), dwOldProtect, &dwOldProtect);

                return TRUE;
            }
            pIATThunk++;
        } while ((ULONG64)pIATThunk[0].u1.AddressOfData);

        pImportTable++;
    } while (pImportTable[0].Characteristics ||
        pImportTable[0].FirstThunk ||
        pImportTable[0].ForwarderChain ||
        pImportTable[0].Name ||
        pImportTable[0].OriginalFirstThunk ||
        pImportTable[0].TimeDateStamp);

    return FALSE;
}

