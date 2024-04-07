// PETool.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "PETool.h"


#define MAX_LOADSTRING 100

// 进程列表
vector<ProcessInfo>* pInfoList = nullptr;

HINSTANCE hAppInstance = nullptr;
HWND handDlgMain = nullptr;


// 初始列表(设置表头)
VOID InitProcessList(HWND hDig);
// 列表中插入 数据
VOID InsertProcessItem(HWND hDig);

// 初始列表(设置表头)
VOID InitModuleList(HWND hDig);
// 列表中插入 数据
VOID InsertModuleItem(HWND hDig);

INT_PTR CALLBACK DialogMainProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);

INT_PTR CALLBACK DialogPEProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
);


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
    DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_MAIN),nullptr, DialogMainProc);

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

    switch (uMsg)
    {
    case WM_INITDIALOG:
        InitProcessList(handDlg);
        InitModuleList(handDlg);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(handDlg, 0);
            return TRUE;

        case IDC_PE_LOAD:  // 打开pe文件，获取路径
            TCHAR szPESuffix[100] = TEXT("Executable Files\0 *.exe;*.dll;*.scr;*.drv;*.sys;");
            TCHAR szFileName[256];
            memset(szFileName,0,256);
            memset(&stOpenFile,0,sizeof stOpenFile);
            stOpenFile.lStructSize = sizeof OPENFILENAME;
            stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            stOpenFile.hwndOwner = handDlg;
            stOpenFile.lpstrFilter = szPESuffix;
            stOpenFile.lpstrFile = szFileName;
            stOpenFile.nMaxFile = MAX_PATH;

            if (GetOpenFileName(&stOpenFile))
            {
                //MessageBox(0, szFileName, 0, MB_OK);
                DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_PE_MAIN), handDlg, nullptr);
            }
            return TRUE;
        }
    case WM_NOTIFY:
        NMCLICK* pNMHDR = (NMCLICK*)lParam;
        if (LOWORD(wParam) == IDC_LIST_PROCESS && pNMHDR->hdr.code == NM_CLICK)
        {
            InsertModuleItem(GetDlgItem(handDlg, IDC_LIST_PROCESS));
        }
        return TRUE;

    }
    return FALSE;
}

INT_PTR CALLBACK DialogPEProc(
    HWND handDlg,  // handle to dialog box		
    UINT uMsg,     // message		
    WPARAM wParam, // first message parameter		
    LPARAM lParam  // second message parameter		
)
{
    switch (uMsg)
    {
    default:
        break;
    }
}


VOID InitProcessList(HWND hDlg)
{
    LV_COLUMN lv;
    HWND hListProcess;

    //初始化								
    //memset(&lv, 0, sizeof(LV_COLUMN));
    //获取IDC_LIST_PROCESS句柄								
    hListProcess = GetDlgItem(hDlg, IDC_LIST_PROCESS);
    //设置整行选中								
    SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //第一列								
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = (PTCHAR)TEXT("进程");				//列标题				
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
    for (auto it = pInfoListRef.begin(); it != pInfoListRef.end(); it++)
    {
        memcpy(name, it->name, 0x50 * sizeof(TCHAR));
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
    LV_COLUMN lv;
    HWND hListModule;

    //初始化								
    //memset(&lv, 0, sizeof(LV_COLUMN));
    //获取IDC_LIST_PROCESS句柄								
    hListModule = GetDlgItem(hDlg, IDC_LIST_MODULE);
    //设置整行选中								
    SendMessage(hListModule, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //第一列								
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = (PTCHAR)TEXT("模块");				//列标题				
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
    TCHAR pIdStr[0x50] = { 0 };
    ULONG64 pId = 0;
    LV_ITEM lv = { 0 };
    vector<ProcessInfo>& pInfoListRef = pInfoList[0];

    // 发送消息 hListProcess窗口，被选中的行号是多少
    dwRowNum = SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    if (dwRowNum == -1)
    {
        MessageBox(0, TEXT("请选择进程！"), TEXT("ERROR"), 0);
        return;
    }

    // 获取pid
    lv.iSubItem = 1;        // 2 columns
    lv.pszText = pIdStr;       // pId
    lv.cchTextMax = 0x50;   // 字段最大长度
    // 获取选中行 pid
    SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowNum, (DWORD)&lv);
    pId = wcstol(pIdStr, 0, 16);
    HWND hListModule = GetDlgItem(handDlgMain, IDC_LIST_MODULE);

    LV_ITEM item = { 0 };
    item.mask = LVIF_TEXT;

    TCHAR name[0x50];
    for (auto pIt = pInfoListRef.begin(); pIt != pInfoListRef.end(); pIt++)
    {
        if (pIt->pid == pId)
        {
            // 迭代器 遍历列表
            for (auto it = pIt->moduleListPtr->begin(); it != pIt->moduleListPtr->end(); it++)
            {
                memcpy(name, it->name, 0x50 * sizeof(TCHAR));
                item.pszText = name;
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

            if (pIt->moduleListPtr->size() == 0) {
                for (int i = 0; i < 300; i++) ListView_DeleteItem(hListModule, 0);
            }
            break;
        }
    }

}



