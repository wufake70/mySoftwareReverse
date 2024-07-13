

INT_PTR CALLBACK DialogPackingProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
)
{
    OPENFILENAME stOpenFile = { 0 };
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
            memset(szFilePath, 0, MAX_PATH);

            GetModuleFileName(hAppInstance, szFilePath, MAX_PATH);
#ifdef _WIN64
            if (IsPE(szFilePath) && IsPE32(szFilePath)==FALSE) // GetOpenFileName(&stOpenFile) &&
            {
                SetWindowText(GetDlgItem(handDlg, IDC_STATIC_SHELL_PATH), szFilePath);
            }
#else
            if (IsPE32(szFilePath)) // GetOpenFileName(&stOpenFile) &&
            {
                SetWindowText(GetDlgItem(handDlg, IDC_STATIC_SHELL_PATH), szFilePath);
            }
#endif // 
            

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

#ifdef _WIN64
            
            if (GetOpenFileName(&stOpenFile) && IsPE(szFilePath) && IsPE32(szFilePath) == FALSE) // GetOpenFileName(&stOpenFile) &&
            {
                SetWindowText(GetDlgItem(handDlg, IDC_STATIC_SRC_PATH),szFilePath);
            }
#else
            if (GetOpenFileName(&stOpenFile) && IsPE32(szFilePath)) // GetOpenFileName(&stOpenFile) &&
            {
                SetWindowText(GetDlgItem(handDlg, IDC_STATIC_SRC_PATH), szFilePath);
            }
#endif
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
            


            // ���� file buffer
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
                // ***ȡ�� ��ѡ ��Դ�Ŀ�ѡPEͷ dll characteristic �� DLL �����ڼ���ʱ�ض�λ***
                GetOptionalHeadersPtr(lpShellFileBuffer)[0].DllCharacteristics &= 0xAF80;

                EncryptData(lpSrcFileBuffer, GetFileSize(hSrcFile, nullptr),lpEncryptedData);
                // ��shell�����ڣ����ܺ�src
                sprintf_s((LPCH)newSectionHeader->Name,8,"%s", ".AAA");
                AddNewSection(&lpShellFileBuffer,
                                GetFileSize(hShellFile,nullptr), 
                                newSectionHeader,
                                lpEncryptedData,
                                GetFileSize(hSrcFile,nullptr));

                /*  ��Ҫshellcode
                    ��shell�ٴ�������,
                    ˫���ӿǺ����
                    ����src����������Ľ���
                */

                // д�����ļ�
                // �����ڣ���Ҫ��֤,���ļ���С >= �����ڴ�С+��foa
                // WriteFile��ReadFile �� bufferSize ����Խ��
                //      win7�� lpNumberOfBytes... ��������ΪNULL
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
            HeapFree(GetProcessHeap(), 0, newSectionHeader);
            return TRUE;
        }

    }

    }

    return FALSE;
}


VOID EncryptData(IN LPBYTE ptrSrcData, DWORD dwSize, OUT LPBYTE ptrEncryptData)
{
    // ��������ʹ�õ���Կ�� 0x55
    BYTE key = 0x55;

    for (DWORD i = 0; i < dwSize; ++i)
    {
        ptrEncryptData[i] = ptrSrcData[i] ^ key;
    }
}

VOID DecryptData(IN LPBYTE ptrSrcData, DWORD dwSize, OUT LPBYTE ptrDecryptData)
{
    // ���ܺͼ���ʹ����ͬ����Կ
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


BOOL UnmapViewOfSection(HANDLE hProcess, PVOID BaseAddress)
{
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (ntdll == NULL) {
        // �������ʧ�ܵ����
        // MessageBox(0, TEXT("FAIL1"), 0, 0);
        return FALSE;
    }

    // ��ȡNtUnmapViewOfSection������ַ
    pNtUnmapViewOfSection NtUnmapViewOfSection = (pNtUnmapViewOfSection)GetProcAddress(ntdll, "NtUnmapViewOfSection");
    if (NtUnmapViewOfSection == NULL) {
        // �����ȡʧ�ܵ����
        //FreeLibrary(ntdll);
        // MessageBox(0, TEXT("FAIL2"), 0, 0);
        return FALSE;
    }

    // ����NtUnmapViewOfSection����
    DWORD status = NtUnmapViewOfSection(hProcess, BaseAddress);
    if (!(status==0)) {
        // �������ʧ�ܵ����
        //FreeLibrary(ntdll);
        // MessageBox(0, TEXT("FAIL3"), 0, 0);
        return FALSE;
    }

    // �ͷ�ntdll.dll
    //FreeLibrary(ntdll);
    // MessageBox(0,TEXT("0K"), 0, 0);
    return TRUE;
}