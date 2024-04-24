


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
    TCHAR szExitCode[0x10] = { 0 };

    DWORD dwPid = 0;
    DWORD dwWriteBytesNum = 0;
    DWORD dwExitCode = 0;

    HANDLE hProcess = nullptr;
    HANDLE hRemoteThread = nullptr;

    LPBYTE lpDllData = nullptr;
    
    switch(uMsg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:

            EndDialog(handDlg, 0);
            return TRUE;

        case IDC_BUTTON_GET_DLL:
            ZeroMemory(szDllPath,MAX_PATH);
            ZeroMemory(szSuffix,0x100);
            wsprintf(szSuffix, TEXT("%s"), TEXT("dll file \0 *.dll;"));
            stOpenFile.lStructSize = sizeof(stOpenFile);
            stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            stOpenFile.hwndOwner = handDlg;
            stOpenFile.lpstrFilter = szSuffix;
            stOpenFile.lpstrFile = szDllPath;
            stOpenFile.nMaxFile = MAX_PATH;

            if (GetOpenFileName(&stOpenFile) && IsPE32(szDllPath))
            {
                SetWindowText(GetDlgItem(handDlg, IDC_STATIC_DLL_PATH), szDllPath);
            }
            return TRUE;

        case IDC_BUTTON_INJECT:
            GetWindowText(GetDlgItem(handDlg, IDC_EDIT_PROCESS_ID), szPid, 0x10);
            dwPid = wcstol(szPid, 0, 16);
            GetWindowText(GetDlgItem(handDlg, IDC_STATIC_DLL_PATH), szDllPath, MAX_PATH);
            if (!IsPE(szDllPath)) return TRUE;

            // 获取目标进程 
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE,dwPid);
            if (!hProcess)
            {
                MessageBox(0,TEXT("OpenProcess Fail."),0,0);
                return TRUE;
            }
            // 在目标进程写入dll地址，便于LoadLibrary加载dll
            lpDllData = (LPBYTE)VirtualAllocEx(hProcess, 
                                             NULL,
                                            (lstrlen(szDllPath)+1)*sizeof(TCHAR),
                                            MEM_COMMIT, 
                                            PAGE_EXECUTE_READWRITE);
            if (!lpDllData)
            {
                MessageBox(0,TEXT("VirtualAllocEx Fail."),0,0);
                CloseHandle(hProcess);
                return TRUE;
            }
            WriteProcessMemory(hProcess,
                lpDllData, szDllPath,
                (lstrlen(szDllPath) + 1) * sizeof(TCHAR),
                &dwWriteBytesNum);
            if (dwWriteBytesNum != (lstrlen(szDllPath) + 1) * sizeof(TCHAR))
            {
                MessageBox(0,TEXT("Dll Path write inot process Fail."),0,0);
                VirtualFreeEx(hProcess, lpDllData, 0, MEM_RELEASE);
                CloseHandle(hProcess);
                return TRUE;
            }

            // 创建远程线程，回调函数为 loadlibrary(该函数存在系统dll中 地址是固定的)，
            // 在将 dll的路径地址作为参数
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
            // 等待线程执行完成
            WaitForSingleObject(hRemoteThread, INFINITE);
            // dwExitCode 就是 注入dll的基址
            GetExitCodeThread(hRemoteThread, &dwExitCode);
            wsprintf(szExitCode, TEXT("0x%x"), dwExitCode);
            SetWindowText(GetDlgItem(handDlg,IDC_STATIC_DLL_IMGBASE), szExitCode);

            VirtualFreeEx(hProcess, lpDllData, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            CloseHandle(hRemoteThread);
            return TRUE;


        }


    }


    return FALSE;
}