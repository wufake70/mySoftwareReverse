// PETool.cpp : 定义应用程序的入口点。
//

//#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "PETool.h"

#ifdef _WIN64
TCHAR szDlgTitle[0x10] = TEXT("PEtool(x64)");
#else
TCHAR szDlgTitle[0x10] = TEXT("PEtool(x86)");
#endif // 


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    /* ***傀儡进程*** */
    // 判断自身是否为 加壳程序
    if (IsPackingEXE(hInstance))
    {
        CHAR currentExePath[MAX_PATH] = { 0 };
        GetModuleFileNameA(hInstance,currentExePath,MAX_PATH);
        
        STARTUPINFOA sI = { 0 };
        PROCESS_INFORMATION pI = { 0 };
        sI.cb = sizeof(sI);

#ifdef _WIN64
        LPBYTE lpDecryptedData = nullptr;
        LPBYTE lpImgBuffer = nullptr;
        LPVOID lpSrcImgBase = nullptr;
        ULONG64 ullImgBase = GetOptionalHeaders64Ptr((LPBYTE)hInstance)->ImageBase;
        DWORD sectionNum = GetFileHeadersPtr((LPBYTE)hInstance)->NumberOfSections - 1;
        SECTION_HEADER* lpLastSection = GetSectionHeadersPtr((LPBYTE)hInstance, sectionNum);

        ULONG64 ullSrcImgBase = 0;
        ULONG64 ullSrcImgSize = 0;
        ULONG64 ullSrcEntryPoint = 0;
        ULONG64 ullShellImage = 0;
        ULONG64 ullWritenByteNum = 0;
        CONTEXT threadContext = { 0 };

        // 创建被挂起的进程
        if (!CreateProcessA(
            currentExePath,
            NULL,
            NULL,
            NULL,
            FALSE,
            CREATE_SUSPENDED,
            NULL,
            NULL,
            &sI,
            &pI)) return 0;

        lpDecryptedData = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, lpLastSection[0].Misc.VirtualSize);
        if (!lpDecryptedData) return 0;

        DecryptData((LPBYTE)(lpLastSection[0].VirtualAddress + (ULONG64)hInstance),
            lpLastSection[0].Misc.VirtualSize,
            lpDecryptedData);

        // 是否解密成功？
        if (!(GetFileHeadersPtr(lpDecryptedData)->Machine == 0x8664)) return 0;
        ullSrcEntryPoint = GetOptionalHeaders64Ptr(lpDecryptedData)->AddressOfEntryPoint;

        // 获取外壳程序的Context
        threadContext.ContextFlags = CONTEXT_FULL;
        GetThreadContext(pI.hThread, &threadContext);

        // 卸载壳子文件镜像
        ReadProcessMemory(pI.hProcess, (LPVOID)(threadContext.Rdx + sizeof(SIZE_T)*2), &ullShellImage, sizeof(SIZE_T), 0);
        if (!(UnmapViewOfSection(pI.hProcess, (PVOID)ullShellImage) == 0))
        {
            CloseHandle(pI.hProcess);
            CloseHandle(pI.hThread);
            return 0;
        }

        // 在挂起的进程中 指定位置 申请内存
        ullSrcImgBase = GetOptionalHeaders64Ptr(lpDecryptedData)->ImageBase;
        ullSrcImgSize = GetOptionalHeaders64Ptr(lpDecryptedData)->SizeOfImage;
        // 可能申请失败
        lpSrcImgBase = VirtualAllocEx(pI.hProcess,
            (LPVOID)ullSrcImgBase,
            ullSrcImgSize,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_EXECUTE_READWRITE);
        // 申请失败，判断 重定位表
        if (!((ULONG64)lpSrcImgBase == ullSrcImgBase))
        {
            MessageBox(0, TEXT("VirtualAlloc Fail. Need to relocate."), 0, 0);
            CloseHandle(pI.hProcess);
            CloseHandle(pI.hThread);
            return 0;
        }
        // 内存拉升
        FileBufferToImageBuffer(lpDecryptedData, lpImgBuffer);
        if (!lpImgBuffer)
        {
            CloseHandle(pI.hProcess);
            CloseHandle(pI.hThread);
            return 0;
        }
        // src imgbuffer 注入进程
        WriteProcessMemory(pI.hProcess, lpSrcImgBase, lpImgBuffer, ullSrcImgSize, &ullWritenByteNum);
        if (!(ullSrcImgSize == ullWritenByteNum))
        {
            CloseHandle(pI.hProcess);
            CloseHandle(pI.hThread);
            return 0;
        }

        // 修改入口信息
        threadContext.Rcx = (ULONG64)lpSrcImgBase + ullSrcEntryPoint;
        ullWritenByteNum = 0;
        WriteProcessMemory(pI.hProcess, (LPVOID)(threadContext.Rdx + sizeof(SIZE_T)*2), &lpSrcImgBase, sizeof(SIZE_T), &ullWritenByteNum);
        SetThreadContext(pI.hThread, &threadContext);

        ResumeThread(pI.hThread);
        CloseHandle(pI.hProcess);
        CloseHandle(pI.hThread);
        HeapFree(GetProcessHeap(), 0, lpDecryptedData);
        HeapFree(GetProcessHeap(), 0, lpImgBuffer);
#else
        LPBYTE lpDecryptedData = nullptr;
        LPBYTE lpImgBuffer = nullptr;
        LPVOID lpSrcImgBase = nullptr;
        DWORD imgBase = GetOptionalHeadersPtr((LPBYTE)hInstance)->ImageBase;
        DWORD sectionNum = GetFileHeadersPtr((LPBYTE)hInstance)->NumberOfSections - 1;
        SECTION_HEADER* lpLastSection = GetSectionHeadersPtr((LPBYTE)hInstance, sectionNum);

        DWORD dwSrcImgBase = 0;
        DWORD dwSrcImgSize = 0;
        DWORD dwSrcEntryPoint = 0;
        DWORD dwShellImage = 0;
        DWORD dwWritenByteNum = 0;
        CONTEXT threadContext = { 0 };

        // 创建被挂起的进程
        if (!CreateProcess(
            currentExePath,
            NULL,
            NULL,
            NULL,
            FALSE,
            CREATE_SUSPENDED,
            nullptr,
            nullptr,
            &sI,
            &pI)) return 0;

        lpDecryptedData = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, lpLastSection[0].Misc.VirtualSize);
        if (!lpDecryptedData) return 0;

        DecryptData((LPBYTE)(lpLastSection[0].VirtualAddress + (DWORD)hInstance),
            lpLastSection[0].Misc.VirtualSize,
            lpDecryptedData);

        // 是否解密成功？
        if (!(GetFileHeadersPtr(lpDecryptedData)->Machine == 0x14C)) return 0;
        dwSrcEntryPoint = GetOptionalHeadersPtr(lpDecryptedData)->AddressOfEntryPoint;

        // 获取外壳程序的Context
        threadContext.ContextFlags = CONTEXT_ALL;
        GetThreadContext(pI.hThread,&threadContext); 

        // 卸载壳子文件镜像
        ReadProcessMemory(pI.hProcess, (LPVOID)(threadContext.Ebx + sizeof(SIZE_T)*2), &dwShellImage, sizeof(SIZE_T), 0);
        //TCHAR aa[0x10] = { 0 };
        //wsprintf(aa,TEXT("%d\0"),dwShellImage);
        //MessageBox(0,aa,0,0);
        if (!(UnmapViewOfSection(pI.hProcess, (PVOID)dwShellImage)==0))
        {
            CloseHandle(pI.hProcess);
            CloseHandle(pI.hThread);
            return 0;
        }

        // 在挂起的进程中 指定位置 申请内存
        dwSrcImgBase = GetOptionalHeadersPtr(lpDecryptedData)->ImageBase ;
        dwSrcImgSize = GetOptionalHeadersPtr(lpDecryptedData)->SizeOfImage;
        // 可能申请失败
        lpSrcImgBase = VirtualAllocEx(pI.hProcess,
                                    (LPVOID)dwSrcImgBase,
                                    dwSrcImgSize,
                                    MEM_COMMIT | MEM_RESERVE, 
                                    PAGE_EXECUTE_READWRITE); // MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
        // 申请失败，判断 重定位表
        if (!((DWORD)lpSrcImgBase == dwSrcImgBase))
        {
            MessageBox(0,TEXT("VirtualAlloc Fail.Need to relocate."),0,0);
            CloseHandle(pI.hProcess);
            CloseHandle(pI.hThread);
            return 0;
        }
        // 内存拉升
        FileBufferToImageBuffer(lpDecryptedData, lpImgBuffer);
        if (!lpImgBuffer)
        {
            CloseHandle(pI.hProcess);
            CloseHandle(pI.hThread);
            return 0;
        }
        // src imgbuffer 注入进程
        WriteProcessMemory(pI.hProcess,lpSrcImgBase,lpImgBuffer,dwSrcImgSize, &dwWritenByteNum);
        if (!(dwSrcImgSize == dwWritenByteNum))
        {
            CloseHandle(pI.hProcess);
            CloseHandle(pI.hThread);
            return 0;
        }

        // 修改入口信息
        threadContext.Eax = (DWORD)lpSrcImgBase + dwSrcEntryPoint;
        dwWritenByteNum = 0;
        WriteProcessMemory(pI.hProcess, (LPVOID)(threadContext.Ebx+ sizeof(SIZE_T) * 2), &lpSrcImgBase, sizeof(SIZE_T), &dwWritenByteNum);
        SetThreadContext(pI.hThread, &threadContext);

        //TCHAR a[0x20] = { 0 };
        //DWORD b = 0;
        //CHAR c[0x20] = { 0 };
        //ReadProcessMemory(pI.hProcess, (LPVOID)(threadContext.Ebx + 8), &b, 4, 0);
        //wsprintf(a,TEXT("eax:%x\nebx:%x\0"), threadContext.Eax,b);
        //MessageBox(0,a,0,0);

        //MessageBox(0, currentExePath, 0, 0);
        ResumeThread(pI.hThread);
        CloseHandle(pI.hProcess);
        CloseHandle(pI.hThread);
        HeapFree(GetProcessHeap(), 0, lpDecryptedData);
        //HeapFree(GetProcessHeap(), 0, lpImgBuffer);
#endif // _WIN64

        return 0;
    }




    /*  ***PETOOL*** */ 
    hAppInstance = hInstance;
    pInfoList = new vector<ProcessInfo>;
    pInfoList[0].reserve(500);
    // 遍历进程信息
    ListProcessesWithModules(pInfoList[0]);

    // 初始化通用控件
    INITCOMMONCONTROLSEX ice = {0};
    ice.dwSize = sizeof(INITCOMMONCONTROLSEX);
    ice.dwICC = ICC_WIN95_CLASSES;  // 指定需要使用的控件
    InitCommonControlsEx(&ice);
    
    // 初始化Dialog
    DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DLG_MAIN),nullptr, DialogMainProc);

    return 0;
}

INT_PTR CALLBACK DialogMainProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
)
{
    handDlgMain = handDlg;
    OPENFILENAME stOpenFile;
    NMCLICK* pNMHDR = { 0 };
    TCHAR szPESuffix[100] = { 0 };
    TCHAR szFileName[256] = { 0 };
#ifdef _WIN64
    ULONG64 dwRowNum = 0;
#else
    DWORD dwRowNum = 0;
#endif // _WIN64
    DWORD ulPid = 0;

    TCHAR szPid[0x10] = { 0 };
    TCHAR szPname[0x50] = { 0 };
    TCHAR szPath[MAX_PATH] = { 0 };
    HWND hListProcess = nullptr;

    LV_ITEM lvItem = { 0 };

    vector<ProcessInfo>& pInfoListRef = pInfoList[0];
    switch (uMsg)
    {
    case WM_INITDIALOG:
        SetWindowText(handDlg, szDlgTitle);
        InitProcessList(handDlg);
        InitModuleList(handDlg);
        return TRUE;

    case WM_CONTEXTMENU:    // 右键菜单
    {
        //// 获取鼠标右键点击位置
        //int xPos = LOWORD(lParam);
        //int yPos = HIWORD(lParam);
        //// 加载菜单资源
        //HMENU hMenu = LoadMenu(hAppInstance, MAKEINTRESOURCE(IDR_MENU));
        //HMENU hContextMenu = GetSubMenu(hMenu, 0);
        //// 显示右键菜单
        //TrackPopupMenu(hContextMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, xPos, yPos, 0, handDlg, NULL);
        //// 销毁菜单资源
        //DestroyMenu(hMenu);
        return TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(handDlg, 0);
            return TRUE;

        case ID_MENU_REFRESH:   // 右键菜单刷新
            pInfoList->clear();
            ListProcessesWithModules(pInfoList[0]);
            InitProcessList(handDlg);
            InitModuleList(handDlg);
            return TRUE;
        case ID_COPY_PID:   // 右键菜单复制
            hListProcess = GetDlgItem(handDlg, IDC_LIST_PROCESS);
            dwRowNum = SendMessage(hListProcess,LVM_GETNEXTITEM,-1,LVNI_SELECTED);
            if (dwRowNum == -1) return TRUE;
            lvItem.iSubItem = 1;
            lvItem.pszText = szPid;
            lvItem.cchTextMax = 0x10;
#ifdef _WIN64
            SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowNum, (ULONG64)&lvItem);
#else
            SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowNum, (DWORD)&lvItem);
#endif // _WIN64
            CopyToClipboard(handDlg,szPid);
            return TRUE;
        case ID_COPY_NAME:   
            hListProcess = GetDlgItem(handDlg, IDC_LIST_PROCESS);
            dwRowNum = SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
            if (dwRowNum == -1) return TRUE;
            lvItem.iSubItem = 0;
            lvItem.pszText = szPname;
            lvItem.cchTextMax = 0x50;
#ifdef _WIN64
            SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowNum, (ULONG64)&lvItem);
#else
            SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowNum, (DWORD)&lvItem);
#endif // _WIN64
            CopyToClipboard(handDlg,&szPname[6]);
            return TRUE;
        case ID_COPY_PATH:   
            hListProcess = GetDlgItem(handDlg, IDC_LIST_PROCESS);
            dwRowNum = SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
            if (dwRowNum == -1) return TRUE;
            lvItem.iSubItem = 1;
            lvItem.pszText = szPid;
            lvItem.cchTextMax = 0x10;
#ifdef _WIN64
            SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowNum, (ULONG64)&lvItem);
#else
            SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowNum, (DWORD)&lvItem);
#endif // _WIN64
            ulPid = wcstol(szPid,0,16);
            pInfoListRef;
            for (auto it = pInfoListRef.begin(); it != pInfoListRef.end();it++)
            {
                if (it[0].pid == ulPid)
                {
                    lstrcat(szPath,it[0].szExePath);
                    break;
                }
            }
            CopyToClipboard(handDlg,szPath);
            return TRUE;

        // PE查看
        case IDC_PE_LOAD:  // 打开pe文件，获取路径
            wsprintf(szPESuffix,TEXT("%s"),TEXT("PE Files\0 *.exe;*.dll;*.scr;*.drv;*.sys;"));
            memset(szFileName,0,256);
            memset(&stOpenFile,0,sizeof stOpenFile);
            stOpenFile.lStructSize = sizeof OPENFILENAME;
            stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            stOpenFile.hwndOwner = handDlg;
            stOpenFile.lpstrFilter = szPESuffix;
            stOpenFile.lpstrFile = szFileName;
            stOpenFile.nMaxFile = MAX_PATH;

            if (GetOpenFileName(&stOpenFile)&&IsPE(szFileName))
            {
                //MessageBox(0, szFileName, 0, MB_OK);
                lstrcpy(PEPath, szFileName);
                DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_PE_MAIN), handDlg, DialogPEProc);

            }
            return TRUE;

        // 程序加壳
        case IDC_PE_PACKING:
#ifdef _WIN64
            DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DLG_PACKING_X64), handDlg, DialogPackingProc);
#else
            DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DLG_PACKING_X86), handDlg, DialogPackingProc);
#endif // _WIN64

            return TRUE;

        // 远程注入
        case IDC_REMOTE_INJECT_DLL:
            DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DLG_INJECTDLL), handDlg, DialogRemoteInJectProc);
            return TRUE;
        }


    case WM_NOTIFY:
        pNMHDR = (NMCLICK*)lParam;
        if (LOWORD(wParam) == IDC_LIST_PROCESS)
        {
            switch (pNMHDR->hdr.code)
            {
            case NM_CLICK:
                InsertModuleItem(GetDlgItem(handDlg, IDC_LIST_PROCESS));
                return TRUE;

            case NM_RCLICK:
                // 获取鼠标右键点击位置
                POINT pt = { 0 };
                GetCursorPos(&pt);

                // 加载菜单资源
                HMENU hMenu = LoadMenu(hAppInstance, MAKEINTRESOURCE(IDR_MENU));

                // 显示右键菜单
                TrackPopupMenu(GetSubMenu(hMenu,0), TPM_LEFTALIGN | TPM_LEFTBUTTON , pt.x, pt.y, 0, handDlg, NULL);

                // 销毁菜单资源
                DestroyMenu(hMenu);

                return TRUE;
            }
        }

    }
    return FALSE;
}




VOID InitProcessList(HWND hDlg)
{
    LV_COLUMN lv = { 0 };
    HWND hListProcess = {0};
    TCHAR szText[0x20] = { 0 };

    //初始化								
    //memset(&lv, 0, sizeof(LV_COLUMN));
    //获取IDC_LIST_PROCESS句柄								
    hListProcess = GetDlgItem(hDlg, IDC_LIST_PROCESS);
    //设置整行选中								
    SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    for (int i = 0; i < 4; i++) ListView_DeleteColumn(hListProcess, 0);
    for (int i = 0; i < 500; i++) ListView_DeleteItem(hListProcess, 0);

#ifdef _WIN64
    wsprintf(szText,TEXT("进程 (%I64d)"),pInfoList[0].size());
#else
    wsprintf(szText,TEXT("进程 (%d)"),pInfoList[0].size());
#endif // _WIN64

    //第一列								
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = szText;				//列标题				
    lv.cx = 220;								//列宽
    lv.iSubItem = 0;                            // 列号
    //ListView_InsertColumn(hListProcess, 0, &lv);								
    SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (LPARAM)&lv);
    //第二列								
    lv.pszText = (PTCHAR)TEXT("PID");
    lv.cx =170;
    lv.iSubItem = 1;
    //ListView_InsertColumn(hListProcess, 1, &lv);								
    SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (LPARAM)&lv);
    //第三列								
    lv.pszText = (PTCHAR)TEXT("镜像基址");
    lv.cx = 170;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListProcess, 2, &lv);
    //第四列								
    lv.pszText = (PTCHAR)TEXT("镜像大小");
    lv.cx = 100;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListProcess, 3, &lv);

    // 插入数据
    InsertProcessItem(hListProcess);

}

VOID InsertProcessItem(HWND hListProcess)
{
    vector<ProcessInfo>& pInfoListRef = pInfoList[0];
    LV_ITEM item = { 0 };
    item.mask = LVIF_TEXT;

    TCHAR name[0x50];
    // 迭代器 遍历列表
    for (auto it = pInfoListRef.rbegin(); it != pInfoListRef.rend(); it++)
    {
#ifdef _WIN64
        wsprintf(name,TEXT("[%03I64d] %s"), distance(it,pInfoListRef.rend()), it->szProName);
#else
        wsprintf(name,TEXT("[%03d] %s"), distance(it,pInfoListRef.rend()), it->szProName);
#endif // _WIN64

        item.pszText = name;
        //wprintf(item.pszText, TEXT("%s"), &it->name);
        item.iItem = 0;     // 行
        item.iSubItem = 0;  // 列
        // 第1行,1列			
        SendMessage(hListProcess, LVM_INSERTITEM, 0, (LPARAM)&item);
        // 2row，2column
        wsprintf(item.pszText, TEXT("0x%X(%d)"), (DWORD)it->pid, (DWORD)it->pid);
        item.iItem = 0;
        item.iSubItem = 1;
        ListView_SetItem(hListProcess, &item);
#ifdef _WIN64
        wsprintf(item.pszText, TEXT("0x%I64X"), it->ulImagebase);
#else
        wsprintf(item.pszText, TEXT("0x%X"), (DWORD)it->ulImagebase);
#endif // _WIN64
        item.iItem = 0;
        item.iSubItem = 2;
        ListView_SetItem(hListProcess, &item);

        wsprintf(item.pszText, TEXT("0x%X"), (DWORD)it->ulImagesize);
        item.iItem = 0;
        item.iSubItem = 3;
        ListView_SetItem(hListProcess, &item);

    }
}


VOID InitModuleList(HWND hDlg)
{
    LV_COLUMN lv = { 0 };
    HWND hListModule = {0};

    //初始化								
    //memset(&lv, 0, sizeof(LV_COLUMN));
    //获取IDC_LIST_PROCESS句柄								
    hListModule = GetDlgItem(hDlg, IDC_LIST_MODULE);
    //设置整行选中								
    SendMessage(hListModule, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    for (int i = 0; i < 4; i++) ListView_DeleteColumn(hListModule, 0);
    for (int i = 0; i < 300; i++) ListView_DeleteItem(hListModule, 0);

    //第一列								
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = (PTCHAR)TEXT("模块 (0)");				//列标题				
    lv.cx = 220;								//列宽
    lv.iSubItem = 0;                            // 列号
    //ListView_InsertColumn(hListProcess, 0, &lv);								
    SendMessage(hListModule, LVM_INSERTCOLUMN, 0, (LPARAM)&lv);

    //第2列								
    lv.pszText = (PTCHAR)TEXT("镜像基址");
    lv.cx = 170;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListModule, 1, &lv);
    //第3列								
    lv.pszText = (PTCHAR)TEXT("镜像大小");
    lv.cx = 100;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListModule, 2, &lv);


}

VOID InsertModuleItem(HWND hListProcess)
{
#ifdef _WIN64
    ULONG64 dwRowNum = 0;
#else
    DWORD dwRowNum = 0;
#endif // _WIN64
    TCHAR pIdStr[0x10] = { 0 };
    ULONG64 pId = 0;
    TCHAR szText[0x50] = {0};
    LV_ITEM lvItem = { 0 };
    LV_COLUMN lvColumn = { 0 };

    vector<ProcessInfo>& pInfoListRef = pInfoList[0];

    // 发送消息 hListProcess窗口，被选中的行号是多少
    dwRowNum = SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    if (dwRowNum == -1)
    {
        MessageBox(0, TEXT("请选择进程！"), TEXT("ERROR"), 0);
        return;
    }

    // 获取pid
    lvItem.iSubItem = 1;        // 2 columns
    lvItem.pszText = pIdStr;       // pId
    lvItem.cchTextMax = 0x50;   // 字段最大长度
    // 获取选中行 pid
#ifdef _WIN64
    SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowNum, (ULONG64)&lvItem);
#else
    SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowNum, (DWORD)&lvItem);
#endif // _WIN64

    pId = wcstol(pIdStr, 0, 16);
    HWND hListModule = GetDlgItem(handDlgMain, IDC_LIST_MODULE);


    LV_ITEM item = { 0 };
    item.mask = LVIF_TEXT;

    //memset(&lvColumn,0,sizeof lvColumn);
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvColumn.pszText = szText;
    lvColumn.cx = 200;								//列宽
    lvColumn.iSubItem = 0;                            // 列号

    for (auto pIt = pInfoListRef.begin(); pIt != pInfoListRef.end(); pIt++)
    {
        if (pIt->pid == pId)
        {
            for (int i = 0; i < 300; i++) ListView_DeleteItem(hListModule, 0);
#ifdef _WIN64
            wsprintf(szText,TEXT("模块 (%I64d)"), pIt->moduleListPtr[0].size());
#else
            wsprintf(szText,TEXT("模块 (%d)"), pIt->moduleListPtr[0].size());
#endif // _WIN64
            SendMessage(hListModule, LVM_SETCOLUMN, 0, (LPARAM)&lvColumn);

            // 迭代器 遍历列表
            for (auto it = pIt->moduleListPtr->rbegin(); it != pIt->moduleListPtr->rend(); it++)
            {
                memcpy(szText, it->szModuleName, 0x50 * sizeof(TCHAR));
#ifdef _WIN64
                wsprintf(szText, TEXT("[%03I64d] %s"), distance(it, pIt->moduleListPtr->rend()), it->szModuleName);
#else
                wsprintf(szText, TEXT("[%03d] %s"), distance(it, pIt->moduleListPtr->rend()), it->szModuleName);
#endif // _WIN64
                item.pszText = szText;
                item.iItem = 0;     // 行
                item.iSubItem = 0;  // 列
                SendMessage(hListModule, LVM_INSERTITEM, 0, (LPARAM)&item);
                // 2row，2column
#ifdef _WIN64
                wsprintf(item.pszText, TEXT("0x%I64X"), it->ulImagebase);
#else
                wsprintf(item.pszText, TEXT("0x%X"), (DWORD)it->ulImagebase);
#endif // _WIN64
                item.iItem = 0;
                item.iSubItem = 1;
                ListView_SetItem(hListModule, &item);

                wsprintf(item.pszText, TEXT("0x%X"), (DWORD)it->ulImagesize);
                item.iItem = 0;
                item.iSubItem = 2;
                ListView_SetItem(hListModule, &item);
            }
            break;
        }
    }

}

