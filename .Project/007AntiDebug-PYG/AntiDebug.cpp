
#include <windows.h>
#include <cstdio>
#include <TlHelp32.h>

#include "ntdll/ntdll.h"
#include "Misc.h"
#include "ApiCheck.h"
#include "WindowsCheck.h"
#include "ModulesCheck.h"
#include "TimeDiffCheck.h"
#include "HwbpCheck.h"
#include <iostream>

#ifdef _WIN64
#pragma comment(lib,"ntdll/ntdll_x64.lib")
#else
#pragma comment(lib,"ntdll/ntdll_x86.lib")
#endif // _WIN64

/*
调用BlockInput(TRUE)，当前线程阻塞时，所有的键盘、鼠标事件会被阻塞。
也只有当前线程调用BlockInput(FALSE)，才能解除。
*/
VOID BlockInputTest();

/*
通过修改api DbgBreakPoint实现反附加
*/
VOID AntiAttachByINT3();

int main()
{
	printf("====================Api Check====================\n");
	_PEBFunc();
	_IsDebuggerPresent();
	_CheckRemoteDebuggerPresent();
	_NtQuerySystemInformation();
	_NtClose();
	_NtQueryInformationProcess();
	_NtDuplicateObject();
	//_ThreadHideFromDebugger(); // 分离调试器
	//_NtQueryObject();	// 不准确

	printf("\n====================Windows Check====================\n");
	auto results1 = CheckDbgWnd();

	for (const auto& res : results1) {
		printf("[%.1f%%] 0x%p - %s\n",
			res.confidence,
			res.hWnd,
			ConvertToUtf8(res.title).c_str());
	}

	printf("\n====================Process Modules(Dll) Check====================\n");
	//auto results2 = CheckProcessModules();
	//for (const auto& res : results2) {
	//	if (res.matchedCount < 3) continue;
	//	printf("%d(pid)\t%s\t%d\n",
	//		res.pid,
	//		ConvertToUtf8(res.mainModule).c_str(),
	//		res.matchedCount);
	//}

	printf("\n====================Time Difference Check====================\n");
	printf("%s调试器\n", TimeDiffCheck1() ? "有" : "无");
	printf("%s调试器\n", TimeDiffCheck2() ? "有" : "无");

	printf("\n====================Test BlockInput====================\n");
	//BlockInputTest();

	printf("\n====================Hardware BreakPoint Check====================\n");
	printf("%s硬件断点\n", HwbpCheck()?"有":"无");
	HwbpCheckByVEH();

	printf("\n====================Test Anti Attach====================\n");
	AntiAttachByINT3();
	while (1);

	system("pause>nul");

	return 0;
}

VOID BlockInputTest()
{
	loop:

	BlockInput(TRUE);
	__try {
		NtClose(ULongToHandle(0x121));
	}
	__except (EXCEPTION_INVALID_HANDLE == GetExceptionCode() ? 
		EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_EXECUTION)
	{
		printf("有调试器...\n");
		goto loop;
	}
	BlockInput(FALSE);
}

VOID AntiAttachByINT3()
{
	PVOID	p = GetProcAddress(GetModuleHandle(L"ntdll"), "DbgBreakPoint");

	byte	retn = 0xC3;

	WriteProcessMemory(NtCurrentProcess,
		p,
		&retn,
		1,
		NULL);
}