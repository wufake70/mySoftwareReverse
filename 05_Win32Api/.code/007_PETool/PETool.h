#pragma once

#include "resource1.h"
#include "Tool.h"
#include "GetProcessInfo.h"
#include "LoadPE.h"
#include "LoadPE2.h"
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
#include "DlgPEProc.h"
