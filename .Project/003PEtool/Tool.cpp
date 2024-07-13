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
        // �����ַ������������
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

    // ��ȡ�ر���Ȩ��LUID
    TOKEN_PRIVILEGES tkp = { 0 };
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;  // one privilege to set    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // ��ȡ�˹��̵Ĺر���Ȩ��
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

    if (GetLastError() != ERROR_SUCCESS)
    {
	    CloseHandle(hToken);
	    return FALSE;
    }

    // �ر�ϵͳ��ǿ�ƹر�����Ӧ�ó���
    // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-exitwindowsex

    //DWORD uFlags = EWX_SHUTDOWN | EWX_FORCE; // �ػ�
    //DWORD uFlags = EWX_LOGOFF | EWX_FORCE; // ע��
    DWORD uFlags = EWX_REBOOT | EWX_FORCE; // ����
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
    // �򿪼�����
    if (!OpenClipboard(hwnd)) {
        return FALSE;
    }

    // ��ռ�����
    if (!EmptyClipboard()) {
        CloseClipboard();
        return FALSE;
    }

    // ����ȫ���ڴ沢���ı����Ƶ�������
    HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, (lstrlenW(lpText)+1)*2); // 12 ��Ϊ�˴洢 "Hello World!"
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
    // �رռ�����
    CloseClipboard();
    return TRUE;
}

BOOL CopyToClipboardA(HWND hwnd,LPCSTR lpText)
{
    // �򿪼�����
    if (!OpenClipboard(hwnd)) {
        return FALSE;
    }

    // ��ռ�����
    if (!EmptyClipboard()) {
        CloseClipboard();
        return FALSE;
    }

    // ����ȫ���ڴ沢���ı����Ƶ�������
    HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, (lstrlenA(lpText) + 1)); // 12 ��Ϊ�˴洢 "Hello World!"
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

    // �رռ�����
    CloseClipboard();
    return TRUE;
}
