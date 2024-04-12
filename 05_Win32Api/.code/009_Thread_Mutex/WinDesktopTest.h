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

DWORD dwMoney = 0;
// 内核对象
HANDLE mutexMoney = nullptr;

HANDLE hThread1 = nullptr;
HANDLE hThread2 = nullptr;
HANDLE hThread3 = nullptr;
HANDLE hThread4 = nullptr;
CRITICAL_SECTION cs;

// thread process 
DWORD WINAPI ThreadProc1(
    LPVOID lpParameter   // thread data		
);
DWORD WINAPI ThreadProc2(
    LPVOID lpParameter   // thread data		
);

DWORD WINAPI ThreadProc3(
    LPVOID lpParameter   // thread data		
);

DWORD WINAPI ThreadProc4(
    LPVOID lpParameter   // thread data		
);

//#include "ACriticalSection.cpp" // 临界区
#include "BMutex.cpp"           // 互斥体
