#include <stdio.h>

#ifndef  TOOL_H
#define TOOL_H


void __cdecl OutputDebugStringFA(const char* format, ...);
void __cdecl OutputDebugStringFW(const wchar_t* format, ...);

// ExitWindowsEx
BOOL ExitWin();


#ifdef _DEBUG  

#ifdef  UNICODE		// unicode  环境
#define DbgPrintf   OutputDebugStringFW
#else				// 多字节字符 环境
#define DbgPrintf   OutputDebugStringFA
#endif

#else  
#define DbgPrintf  
#endif 

#endif // ! TOOL_H

#include "Tool.cpp"

