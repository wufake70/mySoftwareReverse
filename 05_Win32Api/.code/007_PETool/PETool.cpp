// PETool.cpp : 定义应用程序的入口点。
//

#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "PETool.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
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

    switch (uMsg)
    {
    case WM_INITDIALOG:
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
        case ID_MENU_REFRESH:
            pInfoList->clear();
            ListProcessesWithModules(pInfoList[0]);
            InitProcessList(handDlg);
            InitModuleList(handDlg);
            return TRUE;

        case IDC_PE_LOAD:  // 打开pe文件，获取路径
            TCHAR szPESuffix[100] = TEXT("PE Files\0 *.exe;*.dll;*.scr;*.drv;*.sys;");
            TCHAR szFileName[256];
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
                // 获取第一个menu
                HMENU hContextMenu = GetSubMenu(hMenu, 0);

                // 显示右键菜单
                TrackPopupMenu(hContextMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, handDlg, NULL);

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

    wsprintf(szText,TEXT("进程 (%d)"),pInfoList[0].size());
    //第一列								
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = szText;				//列标题				
    lv.cx = 200;								//列宽
    lv.iSubItem = 0;                            // 列号
    //ListView_InsertColumn(hListProcess, 0, &lv);								
    SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (LPARAM)&lv);
    //第二列								
    lv.pszText = (PTCHAR)TEXT("PID");
    lv.cx = 100;
    lv.iSubItem = 1;
    //ListView_InsertColumn(hListProcess, 1, &lv);								
    SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (LPARAM)&lv);
    //第三列								
    lv.pszText = (PTCHAR)TEXT("镜像基址");
    lv.cx = 100;
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
        wsprintf(name,TEXT("[%03d] %s"), distance(it,pInfoListRef.rend()), it->name);
        item.pszText = name;
        //wprintf(item.pszText, TEXT("%s"), &it->name);
        item.iItem = 0;     // 行
        item.iSubItem = 0;  // 列
        // 第1行,1列			
        SendMessage(hListProcess, LVM_INSERTITEM, 0, (LPARAM)&item);
        // 2row，2column
        wsprintf(item.pszText, TEXT("0x%X"), (DWORD)it->pid);
        item.iItem = 0;
        item.iSubItem = 1;
        ListView_SetItem(hListProcess, &item);

        wsprintf(item.pszText, TEXT("0x%X"), (DWORD)it->imagebase);
        item.iItem = 0;
        item.iSubItem = 2;
        ListView_SetItem(hListProcess, &item);

        wsprintf(item.pszText, TEXT("0x%X"), (DWORD)it->imagesize);
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
    lv.cx = 200;								//列宽
    lv.iSubItem = 0;                            // 列号
    //ListView_InsertColumn(hListProcess, 0, &lv);								
    SendMessage(hListModule, LVM_INSERTCOLUMN, 0, (LPARAM)&lv);

    //第2列								
    lv.pszText = (PTCHAR)TEXT("镜像基址");
    lv.cx = 100;
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
    DWORD dwRowNum = 0;
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
    SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowNum, (DWORD)&lvItem);

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

            wsprintf(szText,TEXT("模块 (%d)"), pIt->moduleListPtr[0].size());
            SendMessage(hListModule, LVM_SETCOLUMN, 0, (LPARAM)&lvColumn);

            // 迭代器 遍历列表
            for (auto it = pIt->moduleListPtr->rbegin(); it != pIt->moduleListPtr->rend(); it++)
            {
                memcpy(szText, it->name, 0x50 * sizeof(TCHAR));
                wsprintf(szText, TEXT("[%03d] %s"), distance(it, pIt->moduleListPtr->rend()), it->name);
                item.pszText = szText;
                item.iItem = 0;     // 行
                item.iSubItem = 0;  // 列
                SendMessage(hListModule, LVM_INSERTITEM, 0, (LPARAM)&item);
                // 2row，2column
                wsprintf(item.pszText, TEXT("0x%X"), (DWORD)it->imagebase);
                item.iItem = 0;
                item.iSubItem = 1;
                ListView_SetItem(hListModule, &item);

                wsprintf(item.pszText, TEXT("0x%X"), (DWORD)it->imagesize);
                item.iItem = 0;
                item.iSubItem = 2;
                ListView_SetItem(hListModule, &item);
            }
            break;
        }
    }

}

