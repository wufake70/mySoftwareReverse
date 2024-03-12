// mydll.h: interface for the mydll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYDLL_H__D310B296_259C_4109_9313_61F8839F93A2__INCLUDED_)
#define AFX_MYDLL_H__D310B296_259C_4109_9313_61F8839F93A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

// export 
extern "C" __declspec(dllexport) int __stdcall test(int x,int y);

#ifdef __cplusplus
extern "C++" {
#endif

	__declspec(dllexport) void __stdcall MessageBox();

#ifdef __cplusplus
}
#endif

#endif // !defined(AFX_MYDLL_H__D310B296_259C_4109_9313_61F8839F93A2__INCLUDED_)
