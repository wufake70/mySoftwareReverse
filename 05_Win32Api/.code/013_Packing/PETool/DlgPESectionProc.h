#pragma once



INT_PTR CALLBACK DlgPESectionProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
) 
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
        InitPESectionList(handDlg);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {

        case IDCANCEL:
            EndDialog(handDlg,0);
            return TRUE;

        }

    //case WM_NOTIFY:

    }

    return FALSE;
}


VOID InitPESectionList(HWND hDlg)
{
    LV_COLUMN lv = { 0 };
    HWND hListSection = {0};
 					
    hListSection = GetDlgItem(hDlg, IDC_LIST_SECTION);
    //设置整行选中								
    SendMessage(hListSection, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //第一列								
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = (PTCHAR)TEXT("Name");				//列标题				
    lv.cx = 100;								//列宽
    lv.iSubItem = 0;                            // 列号
    //ListView_InsertColumn(hListProcess, 0, &lv);								
    SendMessage(hListSection, LVM_INSERTCOLUMN, 0, (LPARAM)&lv);
    //第二列								
    lv.pszText = (PTCHAR)TEXT("Virtual Size");
    lv.cx = 100;
    lv.iSubItem = 1;
    //ListView_InsertColumn(hListProcess, 1, &lv);								
    SendMessage(hListSection, LVM_INSERTCOLUMN, 1, (LPARAM)&lv);
    //第三列								
    lv.pszText = (PTCHAR)TEXT("Virtual Offset");
    lv.cx = 100;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListSection, 2, &lv);
    //第四列								
    lv.pszText = (PTCHAR)TEXT("Raw Size");
    lv.cx = 100;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListSection, 3, &lv);

    //第4列								
    lv.pszText = (PTCHAR)TEXT("Raw Offset");
    lv.cx = 100;
    lv.iSubItem = 4;
    ListView_InsertColumn(hListSection, 4, &lv);

    //第5列								
    lv.pszText = (PTCHAR)TEXT("Characteristic");
    lv.cx = 100;
    lv.iSubItem = 5;
    ListView_InsertColumn(hListSection, 5, &lv);


    // 插入数据
    InsertPESectionItem(hListSection);
}

VOID InsertPESectionItem(HWND hDlg)
{
    LV_ITEM item = { 0 };
    VOID* headerPtr = nullptr;
    DWORD dwSectionNum = 0;
    TCHAR text[0x20] = { 0 };
    HMODULE hPeFile = (HMODULE)((BYTE*)LoadLibraryEx(PEPath, 0,
                        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE)-2);
    if (hPeFile == nullptr) return;
    dwSectionNum = GetFileHeadersPtr((BYTE*)hPeFile)->NumberOfSections;
    item.mask = LVIF_TEXT;

    for (int i = dwSectionNum-1; i >= 0; i--)
    {
        headerPtr = GetSectionHeadersPtr((BYTE*)hPeFile, i);
        MultiByteToWideChar(CP_ACP,0, (const char*)((SECTION_HEADER*)headerPtr)->Name, -1, text, 0x20);
        item.pszText = text;
        item.iItem = 0;
        item.iSubItem = 0;
        SendMessage(hDlg, LVM_INSERTITEM, 0, (LPARAM)&item);

        wsprintf(text, TEXT(" %08X"), ((SECTION_HEADER*)headerPtr)->Misc.VirtualSize);
        item.pszText = text;
        item.iItem = 0;
        item.iSubItem = 1;
        SendMessage(hDlg, LVM_SETITEM, 1, (LPARAM)&item);

        wsprintf(text, TEXT(" %08X"), ((SECTION_HEADER*)headerPtr)->VirtualAddress);
        item.pszText = text;
        item.iItem = 0;
        item.iSubItem = 2;
        SendMessage(hDlg, LVM_SETITEM, 2, (LPARAM)&item);

        wsprintf(text, TEXT(" %08X"), ((SECTION_HEADER*)headerPtr)->SizeOfRawData);
        item.pszText = text;
        item.iItem = 0;
        item.iSubItem = 3;
        SendMessage(hDlg, LVM_SETITEM, 3, (LPARAM)&item);

        wsprintf(text, TEXT(" %08X"), ((SECTION_HEADER*)headerPtr)->PointerToRawData);
        item.pszText = text;
        item.iItem = 0;
        item.iSubItem = 4;
        SendMessage(hDlg, LVM_SETITEM, 4, (LPARAM)&item);

        wsprintf(text, TEXT(" %08X"), ((SECTION_HEADER*)headerPtr)->Characteristics);
        item.pszText = text;
        item.iItem = 0;
        item.iSubItem = 5;
        SendMessage(hDlg, LVM_SETITEM, 5, (LPARAM)&item);
    }

    FreeLibrary(hPeFile);

}