#include <stdio.h>
void __cdecl OutputDebugStringFA(const char* format, ...);
void __cdecl OutputDebugStringFW(const wchar_t* format, ...);


#ifdef _DEBUG  

#ifdef  UNICODE		// unicode  环境
#define DbgPrintf   OutputDebugStringFW
#else				// 多字节字符 环境
#define DbgPrintf   OutputDebugStringFA
#endif

#else  
#define DbgPrintf  
#endif 

#include "tool.cpp"

