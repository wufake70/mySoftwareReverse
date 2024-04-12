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
    TCHAR szEditText[0x20] = { 0 };

    switch (uMsg)
    {
    case WM_INITDIALOG:
        mutexMoney = CreateMutex(NULL, FALSE, nullptr);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(handDlg, 0);
            return TRUE;

        case IDC_BUTTON_START:
            if (!dwMoney) {
                handEdit = GetDlgItem(handDlg,IDC_EDIT_MONEY);
                GetWindowText(handEdit,szEditText,0x20);
                dwMoney = wcstol(szEditText,0,10);

                if (!dwMoney) return TRUE;

                hThread1 = CreateThread(0,0,::ThreadProc1,(LPVOID)handDlg, 0, 0);
                InitializeCriticalSection(&cs);

            }
            return TRUE;


        }
    }
    return FALSE;
}



