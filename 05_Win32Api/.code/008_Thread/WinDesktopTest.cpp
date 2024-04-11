// WinDesktopTest.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "WinDesktopTest.h"

#define MAX_LOADSTRING 100



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    hAppInstance = hInstance;
    DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_MAIN),nullptr, DlgMainProc);

    return 0;
}

INT_PTR CALLBACK DlgMainProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
)
{
    DWORD threadExitCode = 0;
    HWND hStaticText = nullptr;
    TCHAR szTxet[0x10] = { 0 };
    DWORD dwNum = 0;

    switch (uMsg)
    {
    case WM_INITDIALOG:

        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(handDlg,0);
            return TRUE;

        case ID_OK:
            if (!handThread1 && !handThread2)
            {
                handThread1=CreateThread(
                    nullptr,
                    0,
                    ::ThreadProc1,      // :: 表示全局函数
                    (LPVOID)handDlg,    // 线程回调函数参数
                    0,                  // 控制线程创建的标志,0 创建后立即执行  
                    0);                 // OUT DWORD* ,线程ID
                handThread2=CreateThread(nullptr,0,
                    ::ThreadProc2,
                    (LPVOID)handDlg,
                    0, 0);

            }

            return TRUE;

        case ID_SUSPEND:
                SuspendThread(handThread1);
                SuspendThread(handThread2);
            return TRUE;
            
        case ID_TEXT:   // 修改线程1 context eip 为 MessageBox
                SuspendThread(handThread1);
                threadContext.ContextFlags = CONTEXT_CONTROL;
                GetThreadContext(handThread1, &threadContext);
                threadContext.Eip = (DWORD)MessageBox;
                SetThreadContext(handThread1,&threadContext);
                SuspendThread(handThread2);

            return TRUE;
            
        case ID_RESUME:
            ResumeThread(handThread1);
            ResumeThread(handThread2);
            return TRUE;
            
        case ID_EXIT:
            if (handThread1 && handThread2)
            {
                ::CloseHandle(handThread1);
                handThread1 = nullptr;
                ::CloseHandle(handThread2);
                handThread2 = nullptr;
            }
            return TRUE;
        }

    }

    return FALSE;
}


DWORD WINAPI ThreadProc1(
    LPVOID lpParameter   // thread data		
)
{
    HWND hStaticText = nullptr;
    TCHAR szTxet[0x10] = { 0 };
    DWORD dwNum = 0;

     hStaticText = GetDlgItem((HWND)lpParameter, IDC_STATIC_TEXT1);
    do {
        if (handThread1 == nullptr) ExitThread(1);
        GetWindowText(hStaticText, szTxet, 0x10);
        dwNum = wcstol(szTxet, 0, 10);
        wsprintf(szTxet, TEXT("%03d"), --dwNum);
        SetWindowText(hStaticText, szTxet);
        Sleep(10);

    } while (dwNum > 0);

    return 0;
}

DWORD WINAPI ThreadProc2(
    LPVOID lpParameter   // thread data		
)
{
    HWND hStaticText = nullptr;
    TCHAR szTxet[0x10] = { 0 };
    DWORD dwNum = 0;

     hStaticText = GetDlgItem((HWND)lpParameter, IDC_STATIC_TEXT2);
    do {
        if (handThread1 == nullptr) ExitThread(1);
        GetWindowText(hStaticText, szTxet, 0x10);
        dwNum = wcstol(szTxet, 0, 10);
        wsprintf(szTxet, TEXT("%03d"), ++dwNum);
        SetWindowText(hStaticText, szTxet);
        Sleep(10);

    } while (dwNum < 1000);

    return 0;
}


