#pragma once

#include "resource.h"
#include "Tool.h"
#include "GetProcessInfo.h"
#include "LoadPE.h"

#define MAX_LOADSTRING 100

// 进程列表
vector<ProcessInfo>* pInfoList = nullptr;

HINSTANCE hAppInstance = nullptr;
HWND handDlgMain = nullptr;
HWND handDlgPE = nullptr;


// 初始列表(设置表头)
VOID InitProcessList(HWND hDig);
// 列表中插入 数据
VOID InsertProcessItem(HWND hDig);

// 初始列表(设置表头)
VOID InitModuleList(HWND hDig);
// 列表中插入 数据
VOID InsertModuleItem(HWND hDig);

INT_PTR CALLBACK DialogMainProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);

TCHAR PEPath[MAX_PATH] = { 0 };

INT_PTR CALLBACK DialogPEProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);
VOID InitDialogPE(HWND handDlg, TCHAR* PEpath);

INT_PTR CALLBACK DlgPESectionProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);

INT_PTR CALLBACK DlgPEDirectoryProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);

VOID InitPESectionList(HWND hDlg);
VOID InsertPESectionItem(HWND hDlg);
VOID InitPEDirectoryEntryList(HWND hDlgs);


DWORD IDC_PE_DList[] = {
    IDC_PE_D_EXPORT_RVA,
    IDC_PE_D_EXPORT_SIZE,
    IDC_PE_D_IMPORT_RVA,
    IDC_PE_D_IMPORT_SIZE,
    IDC_PE_D_RES_RVA,
    IDC_PE_D_RES_SIZE,
    IDC_PE_D_ERROR_RVA,
    IDC_PE_D_ERROR_SIZE,
    IDC_PE_D_SAFETY_RVA,
    IDC_PE_D_SAFETY_SIZE,
    IDC_PE_D_RELOCATION_RVA,
    IDC_PE_D_RELOCATION_SIZE,
    IDC_PE_D_DEBUG_RVA,
    IDC_PE_D_DEBUG_SIZE,
    IDC_PE_D_SDATA_RVA,
    IDC_PE_D_SDATA_SIZE,
    IDC_PE_D_GP_RVA,
    IDC_PE_D_GP_SIZE,
    IDC_PE_D_TLS_RVA,
    IDC_PE_D_TLS_SIZE,
    IDC_PE_D_CONFIG_RVA,
    IDC_PE_D_CONFIG_SIZE,
    IDC_PE_D_BOUND_RVA,
    IDC_PE_D_BOUND_SIZE,
    IDC_PE_D_IAD_RVA,
    IDC_PE_D_IAD_SIZE,
    IDC_PE_DELAY_RVA,
    IDC_PE_DELAY_SIZE,
    IDC_PE_COM_RVA,
    IDC_PE_COM_SIZE,
    IDC_PE_RESERVED_RVA,
    IDC_PE_RESERVED_SIZE
};

#include "DlgPESectionProc.h"
#include "DlgPEDirectoryProc.h"
#include "DlgPEProc.h"


// 程序加壳
INT_PTR CALLBACK DialogPackingProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);

// 数据加/解密
VOID EncryptData(IN LPBYTE ptrSrcData,DWORD dwSize,OUT LPBYTE ptrEncryptData);
VOID DecryptData(IN LPBYTE ptrSrcData, DWORD dwSize, OUT LPBYTE ptrDecryptData);

// 判断 加密的数据是否是 exe
BOOL IsPackingEXE(HINSTANCE hInstance);

// 卸载 外壳程序的文件镜像，
// 定义函数指针，从ntdll中加载
typedef DWORD(WINAPI *pNtUnmapViewOfSection)(HANDLE ProcessHandle, PVOID BaseAddress);
DWORD UnmapViewOfSection(HANDLE hProcess, PVOID BaseAddress);
#include "DlgPackingProc.cpp"


// 远程注入
INT_PTR CALLBACK DialogRemoteInJectProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);
BOOL EnableDebugPriv();
VOID FreeLibraryProc(LPBYTE lpDllData);

#include "DlgRemoteInjectDll.cpp"