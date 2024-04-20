

INT_PTR CALLBACK DialogPackingProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
)
{
    OPENFILENAME stOpenFile;
    NMCLICK* pNMHDR = { 0 };
    TCHAR szPESuffix[100] = { 0 };
    TCHAR szFilePath[MAX_PATH] = { 0 };

    HANDLE hShellFile = { 0 };
    HANDLE hSrcFile = { 0 };
    HANDLE hOutFile = { 0 };

    LPBYTE lpShellFileBuffer = nullptr;
    LPBYTE lpSrcFileBuffer = nullptr;
    LPBYTE lpEncryptedData = nullptr;
    LPBYTE lpOutData = nullptr;
    DWORD dwNumOfBytesRead = 0;
    DWORD dwNumOfBytesWrite = 0;

    SECTION_HEADER* newSectionHeader = nullptr;

    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(handDlg,0);
            return TRUE;

        case IDC_BUTTON_SHELL_PATH:
            wsprintf(szPESuffix, TEXT("%s"), TEXT("exe files\0 *.exe;"));
            memset(szFilePath, 0, 256);
            memset(&stOpenFile, 0, sizeof stOpenFile);
            stOpenFile.lStructSize = sizeof OPENFILENAME;
            stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            stOpenFile.hwndOwner = handDlg;
            stOpenFile.lpstrFilter = szPESuffix;
            stOpenFile.lpstrFile = szFilePath;
            stOpenFile.nMaxFile = MAX_PATH;
            GetModuleFileName(hAppInstance, szFilePath, MAX_PATH);
            if (GetOpenFileName(&stOpenFile) && IsPE32(szFilePath)) // GetOpenFileName(&stOpenFile) &&
            {
                SetWindowText(GetDlgItem(handDlg, IDC_STATIC_SHELL_PATH), szFilePath);
            }

            return TRUE;
      
        case IDC_BUTTON_SRC_PATH:
            wsprintf(szPESuffix, TEXT("%s"), TEXT("exe files\0 *.exe;"));
            memset(szFilePath, 0, 256);
            memset(&stOpenFile, 0, sizeof stOpenFile);
            stOpenFile.lStructSize = sizeof OPENFILENAME;
            stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            stOpenFile.hwndOwner = handDlg;
            stOpenFile.lpstrFilter = szPESuffix;
            stOpenFile.lpstrFile = szFilePath;
            stOpenFile.nMaxFile = MAX_PATH;

            if (GetOpenFileName(&stOpenFile) && IsPE32(szFilePath))
            {
                SetWindowText(GetDlgItem(handDlg, IDC_STATIC_SRC_PATH),szFilePath);
            }

            return TRUE;

        case IDC_BUTTON_PACKING:
            GetWindowText(GetDlgItem(handDlg, IDC_STATIC_SHELL_PATH), szFilePath, MAX_PATH);
            if (!IsPE(szFilePath)) return TRUE;
            hShellFile = CreateFile(szFilePath, 
                                    GENERIC_READ,
                                    FILE_SHARE_READ,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);

            GetWindowText(GetDlgItem(handDlg, IDC_STATIC_SRC_PATH), szFilePath, MAX_PATH);
            if (!IsPE(szFilePath))
            {
                CloseHandle(hShellFile);
                return TRUE;
            }
            hSrcFile = CreateFile(szFilePath,
                                GENERIC_READ,
                                FILE_SHARE_READ,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
            ZeroMemory(&szFilePath[(lstrlen(szFilePath)-4)], 4);
            lstrcat(szFilePath,TEXT("_shell.exe"));
            hOutFile = CreateFile(szFilePath,
                                  GENERIC_WRITE, 
                                  0, 
                                  NULL, 
                                  CREATE_ALWAYS, 
                                  FILE_ATTRIBUTE_NORMAL, 
                                  NULL);
            


            // 申请 file buffer
            lpShellFileBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, GetFileSize(hShellFile,nullptr));
            lpSrcFileBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, GetFileSize(hSrcFile,nullptr));
            lpEncryptedData = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, GetFileSize(hSrcFile,nullptr));
            newSectionHeader = (SECTION_HEADER*)HeapAlloc(GetProcessHeap(), 0, 40);
            if (!lpShellFileBuffer || !lpSrcFileBuffer || !lpEncryptedData || !newSectionHeader)
            {
                CloseHandle(hSrcFile);
                CloseHandle(hShellFile);
                CloseHandle(hOutFile);
                if(lpShellFileBuffer) HeapFree(GetProcessHeap(), 0, lpShellFileBuffer);
                if(lpSrcFileBuffer) HeapFree(GetProcessHeap(), 0, lpSrcFileBuffer);
                if(lpEncryptedData) HeapFree(GetProcessHeap(), 0, lpEncryptedData);
                if(newSectionHeader) HeapFree(GetProcessHeap(), 0, newSectionHeader);

                return TRUE;
            }
            memset(lpShellFileBuffer,0, GetFileSize(hShellFile, nullptr));
            memset(lpSrcFileBuffer, 0, GetFileSize(hSrcFile, nullptr));
            memset(lpEncryptedData, 0, GetFileSize(hSrcFile, nullptr));
            memset(newSectionHeader, 0, 40);

            if (ReadFile(hShellFile, lpShellFileBuffer, GetFileSize(hShellFile, nullptr), &dwNumOfBytesRead, nullptr) && \
                ReadFile(hSrcFile, lpSrcFileBuffer, GetFileSize(hSrcFile, nullptr), &dwNumOfBytesRead, nullptr))
            {
                EncryptData(lpSrcFileBuffer, GetFileSize(hSrcFile, nullptr),lpEncryptedData);
                // 在shell新增节，加密后src
                sprintf_s((LPCH)newSectionHeader->Name,8,"%s", ".AAA");
                AddNewSection(&lpShellFileBuffer,
                                GetFileSize(hShellFile,nullptr), 
                                newSectionHeader,
                                lpEncryptedData,
                                GetFileSize(hSrcFile,nullptr));

                /*  需要shellcode
                    在shell再次新增节,
                    双击加壳后程序，
                    解密src，创建挂起的进程
                */

                // 写入新文件
                // 新增节，需要保证,新文件大小 >= 新增节大小+其foa
                // WriteFile、ReadFile 中 bufferSize 不能越界
                //      win7中 lpNumberOfBytes... 参数不能为NULL
                if (WriteFile(hOutFile,
                    lpShellFileBuffer,
                    newSectionHeader[0].SizeOfRawData + GetFileSize(hShellFile, nullptr),
                    &dwNumOfBytesWrite,
                    nullptr))
                {
                    DbgPrintf(TEXT("DEBUG: Writen it completely!"));
                }

            }

            CloseHandle(hSrcFile);
            CloseHandle(hShellFile);
            CloseHandle(hOutFile);
            HeapFree(GetProcessHeap(), 0, lpShellFileBuffer);
            HeapFree(GetProcessHeap(), 0, lpSrcFileBuffer);
            HeapFree(GetProcessHeap(), 0, lpEncryptedData);
            return TRUE;
        }

    }

    }

    return FALSE;
}


VOID EncryptData(IN LPBYTE ptrSrcData, DWORD dwSize, OUT LPBYTE ptrEncryptData)
{
    // 假设我们使用的密钥是 0x55
    BYTE key = 0x55;

    for (DWORD i = 0; i < dwSize; ++i)
    {
        ptrEncryptData[i] = ptrSrcData[i] ^ key;
    }
}

VOID DecryptData(IN LPBYTE ptrSrcData, DWORD dwSize, OUT LPBYTE ptrDecryptData)
{
    // 解密和加密使用相同的密钥
    BYTE key = 0x55;

    for (DWORD i = 0; i < dwSize; ++i)
    {
        ptrDecryptData[i] = ptrSrcData[i] ^ key;
    }
}

BOOL IsPackingEXE(HINSTANCE hInstance)
{
    DWORD sectionNum = GetFileHeadersPtr((LPBYTE)hInstance)->NumberOfSections-1;
    SECTION_HEADER* lastSection = GetSectionHeadersPtr((LPBYTE)hInstance, sectionNum);
    if (strcmp((CHAR*)lastSection[0].Name, ".AAA") == 0)
    {
        return TRUE;
    }
    return FALSE;
}


DWORD UnmapViewOfSection(HANDLE hProcess, PVOID BaseAddress)
{
    HMODULE ntdll = LoadLibrary(TEXT("ntdll.dll"));
    if (ntdll == NULL) {
        // 处理加载失败的情况
        // MessageBox(0, TEXT("FAIL1"), 0, 0);
        return 1;
    }

    // 获取NtUnmapViewOfSection函数地址
    pNtUnmapViewOfSection NtUnmapViewOfSection = (pNtUnmapViewOfSection)GetProcAddress(ntdll, "NtUnmapViewOfSection");
    if (NtUnmapViewOfSection == NULL) {
        // 处理获取失败的情况
        FreeLibrary(ntdll);
        // MessageBox(0, TEXT("FAIL2"), 0, 0);
        return 1;
    }

    // 调用NtUnmapViewOfSection函数
    DWORD status = NtUnmapViewOfSection(hProcess, BaseAddress);
    if (!(status==0)) {
        // 处理调用失败的情况
        FreeLibrary(ntdll);
        // MessageBox(0, TEXT("FAIL3"), 0, 0);
        return 1;
    }

    // 释放ntdll.dll
    FreeLibrary(ntdll);
    // MessageBox(0,TEXT("0K"), 0, 0);
    return 0;
}