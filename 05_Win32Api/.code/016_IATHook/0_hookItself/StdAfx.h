// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//






#if !defined(AFX_STDAFX_H__3AE8E0C0_5F1D_46D1_A89F_D354824D2102__INCLUDED_)
#define AFX_STDAFX_H__3AE8E0C0_5F1D_46D1_A89F_D354824D2102__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winnt.h>	// ±ÿ–Îµº»Îwindows.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define DOS_HEADER _IMAGE_DOS_HEADER
#define NT_HEADERS _IMAGE_NT_HEADERS
#define NT_HEADERS64 _IMAGE_NT_HEADERS64
#define FILE_HEADER _IMAGE_FILE_HEADER
#define OPTIONAL_HEADER _IMAGE_OPTIONAL_HEADER
#define OPTIONAL_HEADER64 _IMAGE_OPTIONAL_HEADER64
#define SECTION_HEADER _IMAGE_SECTION_HEADER
#define DATA_DIRECTORY _IMAGE_DATA_DIRECTORY 
#define EXPORT_DIRECTORY _IMAGE_EXPORT_DIRECTORY
#define BASE_RELOCATION _IMAGE_BASE_RELOCATION 
#define IMPORT_DESCRIPTOR _IMAGE_IMPORT_DESCRIPTOR
#define THUNK_DATA32 _IMAGE_THUNK_DATA32
#define THUNK_DATA64 _IMAGE_THUNK_DATA64
#define IMPORT_BY_NAME _IMAGE_IMPORT_BY_NAME
#define BOUND_IMPORT_DESCRIPTOR _IMAGE_BOUND_IMPORT_DESCRIPTOR
#define BOUND_FORWARDER_REF _IMAGE_BOUND_FORWARDER_REF
#define RESOURCE_DIRECTORY _IMAGE_RESOURCE_DIRECTORY
#define RESOURCE_DIRECTORY_ENTRY _IMAGE_RESOURCE_DIRECTORY_ENTRY
#define RESOURCE_DIR_STRING_U _IMAGE_RESOURCE_DIR_STRING_U
#define RESOURCE_DATA_ENTRY _IMAGE_RESOURCE_DATA_ENTRY
#define OUT

#endif // _MSC_VER > 1000


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__3AE8E0C0_5F1D_46D1_A89F_D354824D2102__INCLUDED_)
