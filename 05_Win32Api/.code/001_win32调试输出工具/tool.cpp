
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