#include <Windows.h>


// 通过程序路径创建(param1)
VOID TestCreateProcessByAPPName()
{
	// 必须初始化si,且给si的cb成员赋值 大小为 sizeof(si)，方便后面拓展
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(si);

	TCHAR szApplicationName[] = TEXT("c://program files//internet explorer//iexplore.exe");

	BOOL res = CreateProcess(
		szApplicationName,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi);

	// 检查是否成功创建进程
	if (res)
	{
		// 进程创建成功，关闭句柄
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

}

// 通过cmd参数创建(param2)
VOID TestCreateProcessByCmdline()
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(si);

	TCHAR szCmdline[] = TEXT("c://program files//internet explorer//iexplore.exe http://www.ifeng.com");

	BOOL res = CreateProcess(
		NULL,
		szCmdline,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi);

	// 检查是否成功创建进程
	if (res)
	{
		// 进程创建成功，关闭句柄
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

// 通过param1,param2 创建
VOID TestCreateProcess()
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(si);

	TCHAR szCmdline[] = TEXT(" http://www.ifeng.com");

	BOOL res = CreateProcess(
		TEXT("c://program files//internet explorer//iexplore.exe"),
		szCmdline,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi);

	// 检查是否成功创建进程
	if (res)
	{
		// 进程创建成功，关闭句柄
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}


// 
VOID TestCreateProcessCloneMe()
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(si);

	TCHAR szCmdline[0x10] = { 0 };
	wsprintf(szCmdline,TEXT(" %d"), (DWORD)hThread);

	BOOL res = CreateProcess(
		TEXT("C:\\Users\\yui\\source\\repos\\WinDesktopTest\\Debug\\WinDesktopTest.exe"),
		szCmdline,
		NULL,
		NULL,
		TRUE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi);

	// 检查是否成功创建进程
	if (res)
	{
		// 进程创建成功，关闭句柄
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

DWORD WINAPI ThreadPro(LPVOID lpvoidparam)
{
	while (1)
	{
		MessageBox(0, dlgTitle, TEXT("Tip"), MB_OK);
	}
	return 0;
}
