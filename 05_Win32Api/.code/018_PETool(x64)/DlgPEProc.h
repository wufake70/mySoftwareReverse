#pragma once


INT_PTR CALLBACK DialogPEProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
)
{
    handDlgPE = handDlg;
    

    switch (uMsg)
    {
    case WM_INITDIALOG:
        
        InitDialogPE(handDlgPE, PEPath);
        return TRUE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) 
        {
        case IDCANCEL:
            EndDialog(handDlg, 0);
            handDlgPE = nullptr;
            return TRUE;

        case IDC_PE_SECTION_OPEN:
            DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_PE_SECTION_MAIN), handDlg, DlgPESectionProc);
            return TRUE;

        case IDC_PE_DIRECTORY_OPEN:
            DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_PE_DIRECTORY_MAIN), handDlg, DlgPEDirectoryProc);
            return TRUE;
        }
    }
    return FALSE;
}


VOID InitDialogPE(HWND handDlg, TCHAR* PEpath)
{
    HWND hwndStaticText = nullptr;

    TCHAR text[0x16] = { 0 };


    HMODULE hResModule = LoadLibraryEx(PEpath, 0,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (hResModule) {

        hwndStaticText = GetDlgItem(handDlg, IDC_PE_FILEPATH);
        SetWindowText(hwndStaticText, PEPath);

        VOID* headerPtr=nullptr;
        headerPtr = (VOID*)GetDosHeadersPtr((BYTE*)hResModule-2);
        wsprintf(text,TEXT(" 0x%08X"), ((DOS_HEADER*)headerPtr)->e_csum);
        hwndStaticText = GetDlgItem(handDlg, IDC_PE_CHECKSUM_2);
        SetWindowText(hwndStaticText, text);

        headerPtr = (VOID*)GetFileHeadersPtr((BYTE*)hResModule - 2);
        hwndStaticText = GetDlgItem(handDlg, IDC_PE_MACHINE_2);
        wsprintf(text, TEXT(" 0x%08X"), ((FILE_HEADER*)headerPtr)->Machine);
        SetWindowText(hwndStaticText, text);
    
        hwndStaticText = GetDlgItem(handDlg, IDC_PE_SECTIONNUMS_2);
        wsprintf(text, TEXT(" 0x%08X"), ((FILE_HEADER*)headerPtr)->NumberOfSections);
        SetWindowText(hwndStaticText, text);

        hwndStaticText = GetDlgItem(handDlg, IDC_PE_TIMESTAMP_2);
        wsprintf(text, TEXT(" 0x%08X"), ((FILE_HEADER*)headerPtr)->TimeDateStamp);
        SetWindowText(hwndStaticText, text);
    
        hwndStaticText = GetDlgItem(handDlg, IDC_PE_FEATURE_2);
        wsprintf(text, TEXT(" 0x%08X"), ((FILE_HEADER*)headerPtr)->Characteristics);
        SetWindowText(hwndStaticText, text);
    
        // optional header
        if (((FILE_HEADER*)headerPtr)->Machine == 0x14c)
        {
            headerPtr = (VOID*)GetOptionalHeadersPtr((BYTE*)hResModule - 2);
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_ENTRYPOINT_2);
            wsprintf(text, TEXT(" 0x%X"), ((OPTIONAL_HEADER*)headerPtr)->ImageBase + \
                                        ((OPTIONAL_HEADER*)headerPtr)->AddressOfEntryPoint);
            SetWindowText(hwndStaticText, text);
        
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_CODEBASE_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER*)headerPtr)->BaseOfCode);
            SetWindowText(hwndStaticText, text);
        
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_DATABASE_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER*)headerPtr)->BaseOfData);
            SetWindowText(hwndStaticText, text);
        
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_IMGSIZE_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER*)headerPtr)->SizeOfImage);
            SetWindowText(hwndStaticText, text);
        
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_IMGBASE_2);
            OPTIONAL_HEADER* a = (OPTIONAL_HEADER*)headerPtr;
            wsprintf(text, TEXT(" 0x%X"), ((OPTIONAL_HEADER*)headerPtr)->ImageBase);
            SetWindowText(hwndStaticText, text);
        
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_SECTIONALIGN_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER*)headerPtr)->SectionAlignment);
            SetWindowText(hwndStaticText, text);
        
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_FILEALIGN_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER*)headerPtr)->FileAlignment);
            SetWindowText(hwndStaticText, text);
        
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_HEADERSSIZE_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER*)headerPtr)->SizeOfHeaders);
            SetWindowText(hwndStaticText, text);
        
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_FLAG_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER*)headerPtr)->LoaderFlags);
            SetWindowText(hwndStaticText, text);
        
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_SUBSYSTEM_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER*)headerPtr)->Subsystem);
            SetWindowText(hwndStaticText, text);
        
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_DIRECTORYNUM_2);
#ifdef _WIN64
            wsprintf(text, TEXT(" 0x%08I64X"), sizeof(((OPTIONAL_HEADER*)headerPtr)->DataDirectory)/\
                                            sizeof(((OPTIONAL_HEADER*)headerPtr)->DataDirectory[0]));
#else
            wsprintf(text, TEXT(" 0x%08X"), sizeof(((OPTIONAL_HEADER*)headerPtr)->DataDirectory) / \
                sizeof(((OPTIONAL_HEADER*)headerPtr)->DataDirectory[0]));
#endif // _WIN64

            SetWindowText(hwndStaticText, text);
        }
        else {
            headerPtr = (VOID*)GetOptionalHeadersPtr((BYTE*)hResModule - 2);
            hwndStaticText = GetDlgItem(handDlg, IDC_PE_ENTRYPOINT_2);
            OPTIONAL_HEADER64 a = ((OPTIONAL_HEADER64*)headerPtr)[0];
            wsprintf(text, TEXT(" 0x%I64X"), ((OPTIONAL_HEADER64*)headerPtr)->ImageBase + \
                ((OPTIONAL_HEADER64*)headerPtr)->AddressOfEntryPoint);
            SetWindowText(hwndStaticText, text);

            hwndStaticText = GetDlgItem(handDlg, IDC_PE_CODEBASE_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER64*)headerPtr)->BaseOfCode);
            SetWindowText(hwndStaticText, text);

            hwndStaticText = GetDlgItem(handDlg, IDC_PE_IMGSIZE_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER64*)headerPtr)->SizeOfImage);
            SetWindowText(hwndStaticText, text);

            hwndStaticText = GetDlgItem(handDlg, IDC_PE_IMGBASE_2);
            wsprintf(text, TEXT(" 0x%I64X"), ((OPTIONAL_HEADER64*)headerPtr)->ImageBase);
            SetWindowText(hwndStaticText, text);

            hwndStaticText = GetDlgItem(handDlg, IDC_PE_SECTIONALIGN_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER64*)headerPtr)->SectionAlignment);
            SetWindowText(hwndStaticText, text);

            hwndStaticText = GetDlgItem(handDlg, IDC_PE_FILEALIGN_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER64*)headerPtr)->FileAlignment);
            SetWindowText(hwndStaticText, text);

            hwndStaticText = GetDlgItem(handDlg, IDC_PE_HEADERSSIZE_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER64*)headerPtr)->SizeOfHeaders);
            SetWindowText(hwndStaticText, text);

            hwndStaticText = GetDlgItem(handDlg, IDC_PE_FLAG_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER64*)headerPtr)->LoaderFlags);
            SetWindowText(hwndStaticText, text);

            hwndStaticText = GetDlgItem(handDlg, IDC_PE_SUBSYSTEM_2);
            wsprintf(text, TEXT(" 0x%08X"), ((OPTIONAL_HEADER64*)headerPtr)->Subsystem);
            SetWindowText(hwndStaticText, text);

            hwndStaticText = GetDlgItem(handDlg, IDC_PE_DIRECTORYNUM_2);
#ifdef _WIN64
            wsprintf(text, TEXT(" 0x%08I64X"), sizeof(((OPTIONAL_HEADER64*)headerPtr)->DataDirectory) / \
                sizeof(((OPTIONAL_HEADER64*)headerPtr)->DataDirectory[0]));
#else
            wsprintf(text, TEXT(" 0x%08X"), sizeof(((OPTIONAL_HEADER64*)headerPtr)->DataDirectory) / \
                sizeof(((OPTIONAL_HEADER64*)headerPtr)->DataDirectory[0]));

#endif // 

            SetWindowText(hwndStaticText, text);

        }
        FreeLibrary(hResModule);
    }

}