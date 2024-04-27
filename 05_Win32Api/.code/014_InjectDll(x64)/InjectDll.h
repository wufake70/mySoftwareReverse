#pragma once

#include "resource.h"
#include "LoadPE.h"

INT_PTR CALLBACK DialogMainProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);
BOOL EnableDebugPriv();
VOID FreeLibraryProc(LPBYTE lpDllData);
