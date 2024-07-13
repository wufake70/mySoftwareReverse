

INT_PTR CALLBACK DialogRemoteInJectProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
)
{
    OPENFILENAME stOpenFile = { 0 };
    TCHAR szDllPath[MAX_PATH] = { 0 };
    TCHAR szSuffix[0x100] = { 0 };
    TCHAR szPid[0x10] = { 0 };
    TCHAR szExitCode[0x20] = { 0 };
    TCHAR szModule[MAX_PATH] = { 0 };

    DWORD dwPid = 0;
#ifdef _WIN64
    ULONG64 dwWriteBytesNum = 0;
    ULONG64 dwExitCode = 0;
#else
    DWORD dwWriteBytesNum = 0;
    DWORD dwExitCode = 0;
#endif
    DWORD dwcbNeeded = 0;

    HANDLE hProcess = nullptr;
    HANDLE hRemoteThread = nullptr;
    HMODULE hModuleList[1024] = { 0 };

    LPBYTE lpDllData = nullptr;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:

            EndDialog(handDlg, 0);
            return TRUE;

        case IDC_BUTTON_GET_DLL:
            ZeroMemory(szDllPath, MAX_PATH);
            ZeroMemory(szSuffix, 0x100);
            wsprintf(szSuffix, TEXT("%s"), TEXT("dll file \0 *.dll;"));
            stOpenFile.lStructSize = sizeof(stOpenFile);
            stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            stOpenFile.hwndOwner = handDlg;
            stOpenFile.lpstrFilter = szSuffix;
            stOpenFile.lpstrFile = szDllPath;
            stOpenFile.nMaxFile = MAX_PATH;

#ifdef _WIN64
            if (GetOpenFileName(&stOpenFile) && IsPE(szDllPath) && !IsPE32(szDllPath))
            {
                SetWindowText(GetDlgItem(handDlg, IDC_STATIC_DLL_PATH), szDllPath);
            }
            else if (IsPE32(szDllPath))
            {
                MessageBox(0, TEXT("Can't load 32bit PE"), TEXT("Tip"), 0);
            }


#else
            if (GetOpenFileName(&stOpenFile) && IsPE32(szDllPath))
            {
                SetWindowText(GetDlgItem(handDlg, IDC_STATIC_DLL_PATH), szDllPath);
            }
            else if (IsPE(szDllPath) && !IsPE32(szDllPath))
            {
                MessageBox(0, TEXT("Can't load 64bit PE"), TEXT("Tip"), 0);
            }
#endif
            return TRUE;

        case IDC_BUTTON_INJECT:
            //EnableDebugPriv();
            GetWindowText(GetDlgItem(handDlg, IDC_EDIT_PROCESS_ID), szPid, 0x10);
            dwPid = wcstol(szPid, 0, 10);
            GetWindowText(GetDlgItem(handDlg, IDC_STATIC_DLL_PATH), szDllPath, MAX_PATH);
            if (!IsPE(szDllPath)) return TRUE;

            // ��ȡĿ����� 
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
            if (!hProcess)
            {
                MessageBox(0, TEXT("OpenProcess Fail."), 0, 0);
                return TRUE;
            }
            // ��Ŀ�����д��dll��ַ������LoadLibrary����dll
            lpDllData = (LPBYTE)VirtualAllocEx(hProcess,
                NULL,
                (lstrlen(szDllPath) + 1) * sizeof(TCHAR),
                MEM_COMMIT,
                PAGE_EXECUTE_READWRITE);
            if (!lpDllData)
            {
                MessageBox(0, TEXT("VirtualAllocEx Fail."), 0, 0);
                CloseHandle(hProcess);
                return TRUE;
            }
            WriteProcessMemory(hProcess,
                lpDllData, szDllPath,
                (lstrlen(szDllPath) + 1) * sizeof(TCHAR),
                &dwWriteBytesNum);
            if (dwWriteBytesNum != (lstrlen(szDllPath) + 1) * sizeof(TCHAR))
            {
                MessageBox(0, TEXT("Dll Path write inot process Fail."), 0, 0);
                VirtualFreeEx(hProcess, lpDllData, 0, MEM_RELEASE);
                CloseHandle(hProcess);
                return TRUE;
            }

            // ����Զ���̣߳��ص�����Ϊ loadlibrary(�ú�������ϵͳdll�� ��ַ�ǹ̶���)��
            // �ڽ� dll��·����ַ��Ϊ����
            hRemoteThread = CreateRemoteThread(hProcess,
                NULL,
                0,
                (LPTHREAD_START_ROUTINE)LoadLibrary,
                lpDllData,
                0,
                NULL);
            if (!hRemoteThread)
            {
                MessageBox(0, TEXT("hThread Fail."), 0, 0);
                VirtualFreeEx(hProcess, lpDllData, 0, MEM_RELEASE);
                CloseHandle(hProcess);
                return TRUE;
            }
            // �ȴ��߳�ִ�����
            WaitForSingleObject(hRemoteThread, 1000);
            // dwExitCode ���� ע��dll�Ļ�ַ��x64�� ����ULONG64��ַ��DWORD�ض�
            GetExitCodeThread(hRemoteThread, (LPDWORD)&dwExitCode);

            //����Ŀ�����ģ�飬��ȡ��ע�� dll�Ļ�ַ
            EnumProcessModules(hProcess, hModuleList, sizeof(hModuleList), &dwcbNeeded);
            for (UINT i = 0; i < (DWORD)(dwcbNeeded / sizeof(HMODULE)); i++)
            {
                ZeroMemory(szModule, MAX_PATH * sizeof(TCHAR));
                GetModuleFileNameEx(hProcess, hModuleList[i], szModule, MAX_PATH);
                if (lstrcmp(szModule, szDllPath) != 0) continue;
                dwExitCode = (ULONG64)hModuleList[i];
                break;
            }
#ifdef _WIN64
            wsprintf(szExitCode, TEXT("0x%016I64x"), dwExitCode);
            SetWindowText(GetDlgItem(handDlg, IDC_STATIC_DLL_IMGBASE), szExitCode);
#else
            wsprintf(szExitCode, TEXT("0x%x"), dwExitCode);
            SetWindowText(GetDlgItem(handDlg, IDC_STATIC_DLL_IMGBASE), szExitCode);
#endif

            VirtualFreeEx(hProcess, lpDllData, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            CloseHandle(hRemoteThread);
            return TRUE;

            // ж�� dll
        case IDC_BUTTON_FREE:
            GetWindowText(GetDlgItem(handDlg, IDC_EDIT_PROCESS_ID), szPid, 0x10);
            dwPid = wcstol(szPid, 0, 10);
            GetWindowText(GetDlgItem(handDlg, IDC_STATIC_DLL_PATH), szDllPath, MAX_PATH);
            if (!IsPE(szDllPath)) return TRUE;

            // ��ȡĿ����� 
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
            if (!hProcess)
            {
                MessageBox(0, TEXT("OpenProcess Fail."), 0, 0);
                return TRUE;
            }
            // ��Ŀ�����д��dll��ַ������LoadLibrary����dll
            lpDllData = (LPBYTE)VirtualAllocEx(hProcess,
                NULL,
                (lstrlen(szDllPath) + 1) * sizeof(TCHAR),
                MEM_COMMIT,
                PAGE_EXECUTE_READWRITE);
            if (!lpDllData)
            {
                MessageBox(0, TEXT("VirtualAllocEx Fail."), 0, 0);
                CloseHandle(hProcess);
                return TRUE;
            }
            WriteProcessMemory(hProcess,
                lpDllData, szDllPath,
                (lstrlen(szDllPath) + 1) * sizeof(TCHAR),
                &dwWriteBytesNum);
            if (dwWriteBytesNum != (lstrlen(szDllPath) + 1) * sizeof(TCHAR))
            {
                MessageBox(0, TEXT("Dll Path write inot process Fail."), 0, 0);
                VirtualFreeEx(hProcess, lpDllData, 0, MEM_RELEASE);
                CloseHandle(hProcess);
                return TRUE;
            }

            // ����Զ���̣߳��ص�����Ϊ FreeLibrary(�ú�������ϵͳdll�� ��ַ�ǹ̶���)��
            // �ڽ� dll�Ļ�ַ��Ϊ����
            GetWindowText(GetDlgItem(handDlg, IDC_STATIC_DLL_IMGBASE), szExitCode, 0x20 * sizeof(TCHAR));
#ifdef _WIN64
            dwExitCode = wcstoll(szExitCode, 0, 16);
#else
            dwExitCode = wcstol(szExitCode, 0, 16);
#endif

            hRemoteThread = CreateRemoteThread(hProcess,
                NULL,
                0,
                (LPTHREAD_START_ROUTINE)FreeLibrary,
                (LPVOID)dwExitCode,
                0,
                NULL);
            if (!hRemoteThread)
            {
                MessageBox(0, TEXT("CreateRemoteThread Fail."), 0, 0);
                VirtualFreeEx(hProcess, lpDllData, 0, MEM_RELEASE);
                CloseHandle(hProcess);
                return TRUE;
            }

            // �ȴ��߳�ִ�����
            WaitForSingleObject(hRemoteThread, 1000);
            // dwExitCode ���� ע��dll�Ļ�ַ��x64�� ����ULONG64��ַ��DWORD�ض�
            GetExitCodeThread(hRemoteThread, (LPDWORD)&dwExitCode);

            ZeroMemory(&dwExitCode, sizeof(dwExitCode));
            //����Ŀ�����ģ�飬��ȡ��ע�� dll�Ļ�ַ
            EnumProcessModules(hProcess, hModuleList, sizeof(hModuleList), &dwcbNeeded);
            for (UINT i = 0; i < (DWORD)(dwcbNeeded / sizeof(HMODULE)); i++)
            {
                ZeroMemory(szModule, MAX_PATH * sizeof(TCHAR));
                GetModuleFileNameEx(hProcess, hModuleList[i], szModule, MAX_PATH);
                if (lstrcmp(szModule, szDllPath) != 0) continue;
                dwExitCode = (ULONG64)hModuleList[i];
                break;
            }

#ifdef _WIN64
            wsprintf(szExitCode, TEXT("0x%016I64x"), dwExitCode);
            SetWindowText(GetDlgItem(handDlg, IDC_STATIC_DLL_IMGBASE), szExitCode);
#else
            wsprintf(szExitCode, TEXT("0x%x"), dwExitCode);
            SetWindowText(GetDlgItem(handDlg, IDC_STATIC_DLL_IMGBASE), szExitCode);
#endif

            VirtualFreeEx(hProcess, lpDllData, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            CloseHandle(hRemoteThread);
            return TRUE;
        }


    }


    return FALSE;
}

BOOL EnableDebugPriv()
{
    HANDLE hToken;
    LUID Luid;
    TOKEN_PRIVILEGES tkp = { 0 };

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        printf("��Ȩʧ��\n");
        return FALSE;
    }

    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid))
    {
        CloseHandle(hToken);
        printf("��Ȩʧ��\n");
        return FALSE;
    }
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = Luid;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
    {
        printf("��Ȩʧ��\n");
        CloseHandle(hToken);
        return FALSE;
    }
    else
    {
        printf("��Ȩ�ɹ���\n");
        return TRUE;
    }

}

VOID FreeLibraryProc(LPBYTE lpDllData)
{
    HMODULE hModule = LoadLibrary((LPWSTR)lpDllData);
    if (hModule) FreeLibrary(hModule);
}