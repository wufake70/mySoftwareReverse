
#pragma once


INT_PTR CALLBACK DlgPEDirectoryProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        InitPEDirectoryEntryList(handDlg);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {

        case IDCANCEL:
            EndDialog(handDlg, 0);
            return TRUE;

        }



    }
    return FALSE;
}



VOID InitPEDirectoryEntryList(HWND hDlgs)
{
    HWND hStaticText = nullptr;
    HMODULE hPEFile = (HMODULE)((BYTE*)LoadLibraryEx(PEPath, 0,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE)-2);
    DATA_DIRECTORY* DirectoryPtr = nullptr;
    TCHAR text[0x10] = { 0 };

    //DATA_DIRECTORY Directory = { 0 };
    DirectoryPtr = GetDirectoryEntryPtr((BYTE*)hPEFile, 0);
    

    if (hPEFile)
    {
        for (int i = 0; i < 0x20; i++)
        {
            hStaticText = GetDlgItem(hDlgs, IDC_PE_DList[i]);
            wsprintf(text, TEXT(" %08X"), DirectoryPtr[i/2].VirtualAddress);
            SetWindowText(hStaticText, text);
            hStaticText = GetDlgItem(hDlgs, IDC_PE_DList[++i]);
            wsprintf(text, TEXT(" %08X"), DirectoryPtr[i/2].Size);
            SetWindowText(hStaticText, text);
        }
        FreeLibrary(hPEFile);
    }
    return;
}

