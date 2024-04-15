#pragma once
#include <Commdlg.h>
#include "resource.h"



// 全局变量:
HINSTANCE hAppInstance = nullptr;

INT_PTR CALLBACK DlgMainProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);

TCHAR exePath[MAX_PATH] = { 0 };
TCHAR cd[MAX_PATH] = { 0 };

HWND hDlg = nullptr;
TCHAR dlgTitle[0x10] = { 0 };
SECURITY_ATTRIBUTES sa_p = { 0 };
SECURITY_ATTRIBUTES sa_t = { 0 };
HANDLE hThread = nullptr;
HANDLE g_Event = nullptr;

PROCESS_INFORMATION browserPi = { 0 };

VOID TestCreateProcessByAPPName();
VOID TestCreateProcessByCmdline();
VOID TestCreateProcessCloneMe();


DWORD WINAPI ThreadPro(LPVOID lpvoidparam);
#include "A.cpp"
