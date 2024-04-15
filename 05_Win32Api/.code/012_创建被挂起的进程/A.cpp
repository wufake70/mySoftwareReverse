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
	browserPi = { 0 };
	si.cb = sizeof(si);

	// 允许当前主进程、主线程被继承
	sa_p.nLength = sizeof(sa_p);
	sa_p.lpSecurityDescriptor = nullptr;
	sa_p.bInheritHandle = TRUE;
	sa_t.nLength = sizeof(sa_t);
	sa_t.lpSecurityDescriptor = nullptr;
	sa_t.bInheritHandle = TRUE;

	TCHAR szCmdline[] = TEXT(" ");

	BOOL res = CreateProcess(
		exePath,
		szCmdline,
		&sa_p,
		&sa_t,
		FALSE,
		CREATE_SUSPENDED,
		NULL,
		TEXT("C:\\Users\\yui\\source"), &si, &browserPi);

	// 获取被挂起创建的进程的入口点
	// 读取挂起的进程 主线程上下文，ebp的值就是入口地址(ImgBase+EntryPoint)
	// 该进程的 ebp+8地址存储着 ImaBase(可能大于pe头中 imgbase)，
	CONTEXT threadContext = { 0 };
	threadContext.ContextFlags = CONTEXT_FULL;
	GetThreadContext(browserPi.hThread, &threadContext);
	TCHAR str[0x50] = { 0 };
	DWORD imgbase = 0;
	ReadProcessMemory(browserPi.hProcess, (LPCVOID)(threadContext.Ebx+8),&imgbase,4,0);
	wsprintf(str,TEXT("entryPoint:%x\nimgBase:%x"), threadContext.Eax, imgbase);
	MessageBox(0, str, TEXT("info"), MB_OK);

	// 检查是否成功创建进程
	if (res)
	{
		// 进程创建成功，关闭句柄
		//CloseHandle(browserPi.hProcess);
		//CloseHandle(browserPi.hThread);
	}
}


// 
VOID TestCreateProcessCloneMe()
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(si);

	TCHAR szCmdline[0x20] = { 0 };
	wsprintf(szCmdline,TEXT(" %d %d"), (DWORD)browserPi.hProcess,(DWORD)browserPi.hThread);

	wsprintf(cd,TEXT("%s/%s"),cd,TEXT("WinDesktopTest.exe"));

	BOOL res = CreateProcess(
		cd,
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
