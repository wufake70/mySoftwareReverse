// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#define IDD_NDM_DOWNLOAD 101

typedef LRESULT (CALLBACK *WinProc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

DWORD isDownload = 0;
HWND hNdmMain = NULL;
HWND hDlgNdmTip = NULL;
HMODULE hDllModule = NULL;

DWORD WINAPI ThreadProc(LPVOID lpParameter);
LRESULT CALLBACK HookWinProc(HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);
INT_PTR CALLBACK NDMDownloadProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);

WinProc OldWinProc = NULL;
TCHAR szDownloadTitle[0x100] = { 0 };


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    hDllModule = hModule;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //MessageBox(0,TEXT("dll load!"), TEXT("Tip"), 0);
        CreateThread(NULL,0,ThreadProc,NULL,0,NULL);


    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    Sleep(2000);
    hNdmMain = FindWindow(TEXT("Neat Download Manager 1.4"), NULL);
    if (hNdmMain == NULL)
    {
        MessageBox(0, TEXT("hNdmMain is NULL."), 0, 0);
        return 0;
    }
    OldWinProc = (WinProc)GetWindowLong(hNdmMain, GWL_WNDPROC);
    SetWindowLong(hNdmMain, GWL_WNDPROC, (LONG)HookWinProc);

    return 0;
}


LRESULT CALLBACK HookWinProc(
    HWND hwnd, 
    UINT uMsg, 
    WPARAM wParam, 
    LPARAM lParam)
{
    TCHAR szDbgInfo[0x50] = { 0 };
    TCHAR *szDownloadInfo = NULL;
    DWORD dwWideChars = 0;

    std::wstring wtext;
    std::wregex pattern(L"\r\n4:(.*)\r\n");
    std::wsmatch match;

    switch (uMsg)
    {
    case 0x40e:
        // wParam(utf-8编码字符串) 转 宽字符
        dwWideChars = MultiByteToWideChar(CP_UTF8, 0,(LPCSTR)wParam, -1, NULL, 0);
        szDownloadInfo = new TCHAR[dwWideChars];
        MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)wParam, -1, szDownloadInfo, dwWideChars);
        wtext = szDownloadInfo;
        if (std::regex_search(wtext, match, pattern))
        {
            // 获取下载标题
            //MessageBox(0, match.str().c_str() , TEXT("Tip"), 0);
            wsprintf(szDownloadTitle,TEXT("%s"), match.str().c_str());
        }
        
        //MessageBox(0, TEXT("Capture uMsg 0x40e"), TEXT("Tip"),0);
        // *** 使用dll中资源创建对话框 ***
        // 第一个参数是dll加载的imgbase
        DialogBox(hDllModule, MAKEINTRESOURCE(IDD_NDM_DOWNLOAD),NULL, NDMDownloadProc);

        if (isDownload)
        {
            OldWinProc(hwnd,uMsg,wParam,lParam);
        }

        delete[] szDownloadInfo;
        szDownloadInfo = NULL;
        return TRUE;
    }
    
    return OldWinProc(hwnd,uMsg,wParam,lParam);

}


INT_PTR CALLBACK NDMDownloadProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
)
{

    HWND hEdit = NULL;
    hEdit = GetDlgItem(handDlg, IDC_TITLE);
    switch (uMsg)
    {
    case WM_INITDIALOG:
        //SetForegroundWindow(handDlg);
        SetWindowPos(handDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        hDlgNdmTip = handDlg;
        if (wcslen(szDownloadTitle) != 0)
        {
            SetWindowText(hEdit,&szDownloadTitle[4]);
            ZeroMemory(szDownloadTitle,0x100);
        }
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            isDownload = 0;
            EndDialog(handDlg, 0);
            return TRUE;

        case IDC_DOWNLOAD:
            isDownload = 1;
            EndDialog(handDlg,0);
            return TRUE;

        case IDC_CANCEL:
            isDownload = 0;
            EndDialog(handDlg, 0);
            return TRUE;
        }
        break;
    }

    return FALSE;
}



void __stdcall help()
{

}
