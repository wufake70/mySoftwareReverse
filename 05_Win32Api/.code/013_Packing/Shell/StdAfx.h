// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__2CD97D17_836A_4131_B1EF_7B0EBAABEAEB__INCLUDED_)
#define AFX_STDAFX_H__2CD97D17_836A_4131_B1EF_7B0EBAABEAEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>
#include <windows.h>


DWORD CopyFileBufferToImageBuffer(PVOID pFileBuffer,PVOID* pImageBuffer);
void MyReadFile(PVOID* pFileBuffer,PDWORD BufferLenth, TCHAR* szFilePath);
DWORD FOA_TO_RVA(PVOID FileAddress, DWORD FOA,PDWORD pRVA);
DWORD RVA_TO_FOA(PVOID FileAddress, DWORD RVA, PDWORD pFOA);
DWORD GetSizeOfImage(PVOID pFileBuffer);
DWORD GetImageBase(PVOID pFileBuffer);
DWORD GetRelocationTable(PVOID pFileBuffer);
DWORD GetOep(PVOID pFileBuffer);
void ChangesImageBase(PVOID pFileBuffer, DWORD TempImageBase);
void XorDecodeAAA(char* p_data,DWORD DecodeSize);
void GetSrcDataFromShell(PVOID pFileBufferShell, PVOID* FileBufferSrc, PDWORD FileBufferLength, PDWORD FileBufferImageBase);
PIMAGE_DOS_HEADER GetDosHeadersPtr(LPBYTE buffer);
PIMAGE_FILE_HEADER GetFileHeadersPtr(LPBYTE buffer);
PIMAGE_SECTION_HEADER GetSectionHeadersPtr(LPBYTE buffer,int index);
BOOL IsPackingEXE(LPBYTE hInstance);
#include "MainFun.cpp"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2CD97D17_836A_4131_B1EF_7B0EBAABEAEB__INCLUDED_)
