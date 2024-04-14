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
    if (lstrlen(lpCmdLine)<=1)
    {
        wsprintf(dlgTitle, TEXT("%s"), TEXT("ParentDlg"));
    }
    else {
        hThread = (HANDLE)wcstol(&lpCmdLine[0], 0, 10);
        //MessageBox(0, lpCmdLine,0,0);
        wsprintf(dlgTitle, TEXT("%s"), TEXT("SubDlg"));

    }
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

    switch (uMsg)
    {
    case WM_INITDIALOG:
        hDlg = handDlg;
        SetWindowText(hDlg, dlgTitle);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(handDlg, 0);
            return TRUE;
        case IDC_BUTTON_SUBPROCE:

            //TestCreateProcessByAPPName();
            //TestCreateProcessByCmdline();
            TestCreateProcessCloneMe();

            return TRUE;

        case IDC_BUTTON_START:

            if (!hThread)
            {
                sa = { 0 };
                sa.nLength = sizeof(sa);
                sa.bInheritHandle = TRUE;
                sa.lpSecurityDescriptor = nullptr;

                hThread = CreateThread(&sa, 0, ThreadPro, 0, 0, 0);
            }
            return TRUE;

        case IDC_BUTTON_SUSPENDED:

            if (hThread)
            {
                SuspendThread(hThread);
            }

            return TRUE;

        case IDC_BUTTON_RESUME:

            if (hThread)
            {
                ResumeThread(hThread);
            }

            return TRUE;


        }
    }
    return FALSE;
}


