#pragma once
#include "LoadPE.h"

// IATHook.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#ifdef _WIN64  
#define IATHook IATHook64
BOOL IATHook64(LPVOID lpOldFuncAddr, LPWSTR lpModuleOfFunc, LPVOID lpNewFuncAddr, LPWSTR lpModuleToScan);
#else
#define IATHook IATHook32
BOOL IATHook32(LPVOID lpOldFuncAddr, LPVOID lpNewFuncAddr);
#endif

// MessageBox函数指针，劫持后在继续运行
// 调用约定、参数、返回值必须一致

#ifdef _WIN64  
BOOL IATHook64(LPVOID lpOldFuncAddr, LPWSTR lpModuleOfFunc,LPVOID lpNewFuncAddr,LPWSTR lpModuleToScan)
{

    TCHAR szItselfPath[MAX_PATH] = { 0 };
    //CHAR charModule[0x50] = { 0 };
    DATA_DIRECTORY* pImportTableEntry = NULL;
    IMPORT_DESCRIPTOR* pImportTable = NULL;
    THUNK_DATA64* pIATThunk = NULL;
    DWORD dwOldProtect = 0;
    HMODULE hItself = NULL;

    if (!lpModuleToScan)
    {
        GetModuleFileName(NULL, szItselfPath, MAX_PATH);
        hItself = LoadLibrary(szItselfPath);
    }
    else {
        hItself = LoadLibrary(lpModuleToScan);
    }

    pImportTableEntry = GetDirectoryEntryPtr((LPBYTE)hItself, 1);
    // 导入表一个dll描述信息
    pImportTable = (IMPORT_DESCRIPTOR*)(pImportTableEntry[0].VirtualAddress + (ULONG64)hItself);
    if (pImportTable == NULL) return FALSE;
    do {
        //ZeroMemory(charModule,0x50);
        //if (lpModuleOfFunc)
        //{
        //    WideCharToMultiByte(CP_ACP, 0, lpModuleOfFunc, -1, charModule, 0x50, NULL, NULL);

        //    if ((PCHAR)(pImportTable[0].Name + (ULONG64)hItself)==NULL) return FALSE;
        //    if (_stricmp((PCHAR)(pImportTable[0].Name+(ULONG64)hItself), charModule) != 0)
        //    {
        //        pImportTable++;
        //        if (pImportTable[0].Characteristics ||
        //            pImportTable[0].FirstThunk ||
        //            pImportTable[0].ForwarderChain ||
        //            pImportTable[0].Name ||
        //            pImportTable[0].OriginalFirstThunk ||
        //            pImportTable[0].TimeDateStamp)
        //        {

        //        continue;
        //        }
        //        return FALSE;
        //    }

        //}

        pIATThunk = (THUNK_DATA64*)(pImportTable[0].FirstThunk + (ULONG64)hItself);
        if (!pIATThunk) return FALSE;
        do {
            // 找到MessageBox 地址并修改到指定函数
            if ((ULONG64)lpOldFuncAddr == pIATThunk[0].u1.AddressOfData)
            {
                // 修改内存保护属性为可写
                dwOldProtect = 0;
                VirtualProtect(&pIATThunk[0].u1.AddressOfData, sizeof(ULONG64), PAGE_READWRITE, &dwOldProtect);
                
                // 修改函数地址
                pIATThunk[0].u1.AddressOfData = (ULONG64)lpNewFuncAddr;

                // 还原内存保护属性
                VirtualProtect(&pIATThunk[0].u1.AddressOfData, sizeof(ULONG64), dwOldProtect, &dwOldProtect);

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
#else

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
#endif




