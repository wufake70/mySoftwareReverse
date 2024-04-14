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
    HWND handEdit = nullptr;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        hDlg = handDlg;
        // 初始化信号量，临界区
        InitializeCriticalSection(&cs);
        SemaphoreMaker = CreateSemaphore(0, 2, 2, 0);
        SemaphoreConsumer = CreateSemaphore(0, 0, 2, 0);
        hConsumerText[0] = GetDlgItem(handDlg, IDC_COMSUMER_A);
        hConsumerText[1] = GetDlgItem(handDlg, IDC_COMSUMER_B);
        hConsumerText[2] = GetDlgItem(handDlg, IDC_COMSUMER_C);
        hConsumerText[3] = GetDlgItem(handDlg, IDC_COMSUMER_D);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            CloseHandle(SemaphoreConsumer);
            EndDialog(handDlg, 0);
            return TRUE;

        case IDC_BUTTON_START:
            if (!lstrlen(szEditText))
            {
                handEdit = GetDlgItem(handDlg, IDC_EDIT_RES);
                GetWindowText(handEdit,szEditText,0x30);

                if (!lstrlen(szEditText)) return TRUE;
                hThread1 = CreateThread(0,0,ThreadProc1,(LPVOID)handDlg, 0, 0);
            }
            return TRUE;


        }
    }
    return FALSE;
}


