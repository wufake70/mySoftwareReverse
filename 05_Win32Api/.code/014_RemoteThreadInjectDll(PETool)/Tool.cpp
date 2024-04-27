//#include "Tool.h"


void __cdecl OutputDebugStringFA(const char* format, ...)
{
    va_list vlArgs;
    char* strBuffer = (char*)GlobalAlloc(GPTR, 4096);
    if (!strBuffer) return;

    va_start(vlArgs, format);
    _vsnprintf_s(strBuffer, 4096 - 1, _TRUNCATE, format, vlArgs);
    va_end(vlArgs);

    strcat_s(strBuffer, 4096, "\n");
    OutputDebugStringA(strBuffer);
    GlobalFree(strBuffer);
    return;
}

void __cdecl OutputDebugStringFW(const wchar_t* format, ...)
{
    va_list vlArgs;
    wchar_t* strBuffer = (wchar_t*)GlobalAlloc(GPTR, 4096);
    if (!strBuffer) return;

    size_t formatLength = wcslen(format);
    if (formatLength >= 2048 - 1) {
        // 处理字符串过长的情况
        return;
    }
    va_start(vlArgs, format);
    _vsnwprintf_s(strBuffer, 2048-1 , _TRUNCATE, format, vlArgs);
    va_end(vlArgs);

    wcscat_s(strBuffer, 2048, L"\n");
    OutputDebugStringW(strBuffer);
    GlobalFree(strBuffer);
    return;
}

BOOL ExitWin()
{
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    return(FALSE);

    // 获取关闭特权的LUID
    TOKEN_PRIVILEGES tkp = { 0 };
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;  // one privilege to set    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // 获取此过程的关闭特权。
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

    if (GetLastError() != ERROR_SUCCESS)
    {
	    CloseHandle(hToken);
	    return FALSE;
    }

    // 关闭系统并强制关闭所有应用程序。
    // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-exitwindowsex

    //DWORD uFlags = EWX_SHUTDOWN | EWX_FORCE; // 关机
    //DWORD uFlags = EWX_LOGOFF | EWX_FORCE; // 注销
    DWORD uFlags = EWX_REBOOT | EWX_FORCE; // 重启
    if (!ExitWindowsEx(uFlags,
	    SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
	    SHTDN_REASON_MINOR_UPGRADE |
	    SHTDN_REASON_FLAG_PLANNED))
    {
	    CloseHandle(hToken);
	    return FALSE;
    }
    CloseHandle(hToken);
    return TRUE;
}

BOOL CopyToClipboardW(HWND hwnd,LPCWSTR lpText)
{
    // 打开剪贴板
    if (!OpenClipboard(hwnd)) {
        return FALSE;
    }

    // 清空剪贴板
    if (!EmptyClipboard()) {
        CloseClipboard();
        return FALSE;
    }

    // 分配全局内存并将文本复制到剪贴板
    HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, (lstrlenW(lpText)+1)*2); // 12 是为了存储 "Hello World!"
    if (hClipboardData != NULL) {
        LPWSTR pchData = (LPWSTR)GlobalLock(hClipboardData);
        if (pchData == nullptr) return FALSE;
        ZeroMemory(pchData, (lstrlenW(lpText) + 1) * 2);
        if (pchData != NULL) {
            wcscpy_s(pchData, (lstrlenW(lpText) + 1), lpText);
            GlobalUnlock(hClipboardData);
            SetClipboardData(CF_UNICODETEXT, hClipboardData);
            GlobalFree(hClipboardData);
        }
    }
    // 关闭剪贴板
    CloseClipboard();
    return TRUE;
}

BOOL CopyToClipboardA(HWND hwnd,LPCSTR lpText)
{
    // 打开剪贴板
    if (!OpenClipboard(hwnd)) {
        return FALSE;
    }

    // 清空剪贴板
    if (!EmptyClipboard()) {
        CloseClipboard();
        return FALSE;
    }

    // 分配全局内存并将文本复制到剪贴板
    HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, (lstrlenA(lpText) + 1)); // 12 是为了存储 "Hello World!"
    if (hClipboardData != NULL) {
        LPCSTR pchData = (LPCSTR)GlobalLock(hClipboardData);
        if (pchData == nullptr) return FALSE;
        ZeroMemory((VOID*)pchData, (lstrlenA(lpText) + 1));
        if (pchData != NULL) {
            strcpy_s((LPSTR)pchData, (lstrlenA(lpText) + 1), lpText);
            GlobalUnlock(hClipboardData);
            SetClipboardData(CF_UNICODETEXT, hClipboardData);
            GlobalFree(hClipboardData);
        }
    }

    // 关闭剪贴板
    CloseClipboard();
    return TRUE;
}
