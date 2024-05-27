#include <stdio.h>

#ifndef  TOOL_H
#define TOOL_H


void __cdecl OutputDebugStringFA(const char* format, ...);
void __cdecl OutputDebugStringFW(const wchar_t* format, ...);

// ExitWindowsEx
BOOL ExitWin();
BOOL CopyToClipboardA(HWND hwnd,LPCSTR lpText);
BOOL CopyToClipboardW(HWND hwnd,LPCWSTR lpText);

#ifdef _DEBUG  

#ifdef  UNICODE		// unicode  环境
#define DbgPrintf			OutputDebugStringFW

#else				// 多字节字符 环境
#define DbgPrintf   OutputDebugStringFA

#endif

#else  //_RELEASE
#define DbgPrintf  
#endif 

#ifdef  UNICODE		// unicode  环境
#define CopyToClipboard		 CopyToClipboardW

#else				// 多字节字符 环境
#define CopyToClipboard		 CopyToClipboardA
#endif


#endif // ! TOOL_H

#include "Tool.cpp"

