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

TCHAR szEditText[0x30] = { 0 };

HANDLE hThread1 = nullptr;
HANDLE hThread2 = nullptr;
HANDLE hThread3 = nullptr;
HANDLE hThread4 = nullptr;
HANDLE hThread5 = nullptr;
HANDLE hThread6 = nullptr;

HWND hConsumerText[4] = {0};

TCHAR A = 0;
TCHAR B = 0;
TCHAR* tcharBuffer[2] = { &A,&B };

// 两个 信号量，一个临界区，即可实现
HANDLE SemaphoreMaker = nullptr;
HANDLE SemaphoreConsumer = nullptr;
CRITICAL_SECTION cs;


// thread process 
DWORD WINAPI ThreadProc1(
    LPVOID lpParameter   // thread data		
);

DWORD WINAPI ThreadResAlloc(
    LPVOID lpParameter   // thread data		
);


DWORD WINAPI ThreadConsumer(
    LPVOID lpParameter   // thread data		
);


#include "A.cpp"
