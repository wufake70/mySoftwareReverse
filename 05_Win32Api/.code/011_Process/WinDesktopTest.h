#pragma once

#include "resource.h"



// 全局变量:
HINSTANCE hAppInstance = nullptr;

INT_PTR CALLBACK DlgMainProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);

HWND hDlg = nullptr;
TCHAR dlgTitle[0x10] = { 0 };
SECURITY_ATTRIBUTES sa = { 0 };
HANDLE hThread = nullptr;
HANDLE g_Event = nullptr;

VOID TestCreateProcessByAPPName();
VOID TestCreateProcessByCmdline();
VOID TestCreateProcessCloneMe();


DWORD WINAPI ThreadPro(LPVOID lpvoidparam);
#include "A.cpp"
