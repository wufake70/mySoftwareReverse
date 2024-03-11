// stdafx.h : include file for standard system include files,										
//  or project specific include files that are used frequently, but										
//      are changed infrequently										
//										
										
#if !defined(AFX_STDAFX_H__3AE8E0C0_5F1D_46D1_A89F_D354824D2102__INCLUDED_)										
#define AFX_STDAFX_H__3AE8E0C0_5F1D_46D1_A89F_D354824D2102__INCLUDED_										
										
#if _MSC_VER > 1000										
#pragma once										
										
#include <windows.h>										
#include <winnt.h>	// 必须导入windows.h									
#include <stdio.h>										
#include <stdlib.h>										
#include <string.h>										
										
										
#define dos_head _IMAGE_DOS_HEADER										
#define nt_head _IMAGE_NT_HEADERS										
#define nt64_head _IMAGE_NT_HEADERS64										
#define stdpe_head _IMAGE_FILE_HEADER										
#define optpe_head _IMAGE_OPTIONAL_HEADER										
#define optpe64_head _IMAGE_OPTIONAL_HEADER64										
#define section_head _IMAGE_SECTION_HEADER										
										
#endif // _MSC_VER > 1000										
										
										
// TODO: reference additional headers your program requires here										
										
//{{AFX_INSERT_LOCATION}}										
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.										
										
#endif // !defined(AFX_STDAFX_H__3AE8E0C0_5F1D_46D1_A89F_D354824D2102__INCLUDED_)										
