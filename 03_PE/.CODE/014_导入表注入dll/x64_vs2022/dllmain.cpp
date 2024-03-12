// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

int __stdcall test(int x,int y)
{
    return x + y;
}

void __stdcall MessageBox()
{
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MessageBox(0,0,L"hello",0);
        //ShellExecute(NULL, L"open", L"cmd.exe", NULL, NULL, SW_SHOWNORMAL);
        //ShellExecute(NULL, L"open", L"C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe", L"https://hao.360.com", NULL, SW_SHOWNORMAL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

