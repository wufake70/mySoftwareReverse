// WindowsProject1.cpp : 定义应用程序的入口点。												

#include "framework.h"												
#include "WindowsProject1.h"												

INT_PTR CALLBACK DialogProc(
	HWND hwndDlg,  // handle to dialog box		
	UINT uMsg,     // message		
	WPARAM wParam, // first message parameter		
	LPARAM lParam  // second message parameter		
);

// 当前程序的ImageBase
HINSTANCE hAppInstance = nullptr;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,			// 命令行启动时的参数
	_In_ int       nCmdShow)
{
	hAppInstance = hInstance;
	DialogBox(hAppInstance, (LPCWSTR)IDD_DIALOG1, nullptr, DialogProc);



	return 0;
}

INT_PTR CALLBACK DialogProc(
	HWND handDlg,  // handle to dialog box		
	UINT uMsg,     // message		
	WPARAM wParam, // first message parameter		
	LPARAM lParam  // second message parameter		
)
{
	// 图标变量
	HICON handBigIcon = nullptr;
	HICON handSmallIcon = nullptr;

	HWND handEdit = nullptr;
	HINSTANCE result = nullptr;
	TCHAR url[0x50] = { 0 };

	switch (uMsg)
	{
	case  WM_INITDIALOG:
		// 加载图标资源
		handBigIcon = LoadIcon(hAppInstance, MAKEINTRESOURCE(IDI_ICON_BIG));
		handSmallIcon = LoadIcon(hAppInstance, MAKEINTRESOURCE(IDI_ICON_SMALL));

		// 设置图标
		SendMessage(handDlg, WM_SETICON, ICON_BIG, (DWORD)handBigIcon);
		SendMessage(handDlg, WM_SETICON, ICON_SMALL, (DWORD)handSmallIcon);	// 程序窗口 左上角


		return TRUE;

	case  WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case   IDOK:

			MessageBox(NULL, TEXT("IDC_BUTTON_OK"), TEXT("OK"), MB_OK);

			return TRUE;

		case   IDCANCEL:

			//MessageBox(NULL, TEXT("IDC_BUTTON_OUT"), TEXT("OUT"), MB_OK);

			EndDialog(handDlg, 0);

			return TRUE;

		case	IDC_SHUTDOWN:
			ExitWin();
			return TRUE;

		case	IDC_LOGOUT:
			ExitWin(1);
			return TRUE;

		case	IDC_REBOOT:
			ExitWin(2);
			return TRUE;

		case	IDC_OPEN:
			// 获取子窗口 句柄 (通用的)
			handEdit = GetDlgItem(handDlg,IDC_EDIT1);

			GetWindowText(handEdit, url, 0x50);
			result = ShellExecute(NULL, TEXT("open"), url, NULL, NULL, SW_SHOWNORMAL);
			if ((int)result <= 32)
			{
				// 打开网页失败
				DWORD error = GetLastError();
				// 处理错误
			}
			return TRUE;

			// 获取 系统提供的回调函数
		case IDC_SYSWINPROC:
			handEdit = GetDlgItem(handDlg, IDC_OPEN);
			TCHAR className[0x50] = { 0 };
			WNDCLASS wc = { 0 };
			GetClassName(handEdit, className, 0x50);
			GetClassInfo(hAppInstance, className, &wc);
			DbgPrintf(TEXT("Debug: %s"), className);
			DbgPrintf(TEXT("Debug: %p"), wc.lpfnWndProc);
			wsprintf(className, TEXT("%p"), wc.lpfnWndProc);

			// 顺便写入剪贴板
			OpenClipboard(handDlg);
			HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, 0x50);
			if (!hData) return TRUE;
			PTCHAR pData = (PTCHAR)GlobalLock(hData);
			if (!pData) return TRUE;
			CopyMemory(pData, className, 0x50);
			GlobalUnlock(hData);
			SetClipboardData(CF_UNICODETEXT, hData);
			CloseClipboard();
			MessageBox(0, className, TEXT("System WinProc Address"), 0);
			return TRUE;
		}
	}

	return FALSE;
}

