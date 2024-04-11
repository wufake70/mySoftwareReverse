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

HANDLE handThread1 = nullptr;
HANDLE handThread2 = nullptr;

// thread process 
DWORD WINAPI ThreadProc1(
    LPVOID lpParameter   // thread data		
);
DWORD WINAPI ThreadProc2(
    LPVOID lpParameter   // thread data		
);

CONTEXT threadContext = {0};
