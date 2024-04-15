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
    GetCurrentDirectory(MAX_PATH, cd);
    MessageBox(0,cd,0,0);
    hAppInstance = hInstance;
    if (lstrlen(lpCmdLine)<=1)
    {
        wsprintf(dlgTitle, TEXT("%s"), TEXT("ParentDlg"));
    }
    else {
        if (swscanf_s(lpCmdLine, TEXT("%u %u"), ((DWORD*)&browserPi.hProcess), ((DWORD*)&browserPi.hThread)) == 2)
        {
            //MessageBox(0, lpCmdLine,0,0);

        }
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
    OPENFILENAME stOpenFile;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        hDlg = handDlg;
        SetWindowText(hDlg, dlgTitle);
        if (browserPi.hProcess)
        {
            ShowWindow(GetDlgItem(handDlg, IDC_BUTTON_SUSPENDED), SW_SHOW);
            ShowWindow(GetDlgItem(handDlg, IDC_BUTTON_RESUME), SW_SHOW);
        }
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(handDlg, 0);
            return TRUE;

        case IDC_BUTTON_SUBPROC:
            //TestCreateProcessByAPPName();
            //TestCreateProcessByCmdline();
            TestCreateProcessCloneMe();
            EndDialog(handDlg, 0);
            return TRUE;
            
        case IDC_BUTTON_START:
            if (!browserPi.hProcess && lstrlen(exePath))
            {
                //TestCreateProcessByAPPName();
                //TestCreateProcessByCmdline();
                //TestCreateProcessCloneMe();
                TestCreateProcess();
                ShowWindow(GetDlgItem(handDlg, IDC_BUTTON_SUBPROC),SW_SHOW);

            }        

            return TRUE;

        case IDC_BUTTON_SUSPENDED:
            SuspendThread(browserPi.hThread);
            return TRUE;

        case IDC_BUTTON_RESUME:
            ResumeThread(browserPi.hThread);
            return TRUE;

        case IDC_BUTTON_PATH:
            TCHAR szPESuffix[100] = TEXT("PE Files\0 *.exe;");
            TCHAR szFileName[256];
            memset(szFileName, 0, 256);
            memset(&stOpenFile, 0, sizeof stOpenFile);
            stOpenFile.lStructSize = sizeof OPENFILENAME;
            stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            stOpenFile.hwndOwner = handDlg;
            stOpenFile.lpstrFilter = szPESuffix;
            stOpenFile.lpstrFile = szFileName;
            stOpenFile.nMaxFile = MAX_PATH;

            if (GetOpenFileName(&stOpenFile))
            {
                wsprintf(exePath,TEXT("%s"), szFileName);
            }

            return TRUE;
        }
    }
    return FALSE;
}


