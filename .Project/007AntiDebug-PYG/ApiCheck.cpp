#include "ApiCheck.h"
#include "ntdll/ntdll.h"


// PEB的四个标志位
void _PEBFunc()
{
	PEB* peb;
#ifdef _WIN64
	peb = (PEB*)__readgsqword(0x60); // x64 中 PEB 地址为 GS:[0x60]
#else
	__asm {
		mov eax, dword ptr fs : [0x30] ;
		mov peb, eax;
	}
#endif _WIN64 //PEB->ProcessHeap

	OUTMESSAGE(__FUNCTION__, peb->BeingDebugged);
	OUTMESSAGE(__FUNCTION__, peb->NtGlobalFlag & 0x70);

	DWORD num = peb->NumberOfHeaps;
	printf("PEB->ProcessHeap\n");
#ifdef _WIN64
	for (int offset = 0; offset < num; offset++)
	{
		//printf("\t0x%x\n\t0x%x\n",
		//	*(DWORD*)((LONG64)peb->ProcessHeap + 0x70),
		//	*(DWORD*)((LONG64)peb->ProcessHeap + 0x74));

		printf("\t[%d] 0x%x 0x%x\n", offset,
			*(LONG64*)(((LONG64*)peb->ProcessHeaps)[offset] + 0x70),
			*(LONG64*)(((LONG64*)peb->ProcessHeaps)[offset] + 0x74));
	}
#else
	//printf("\n0x%x\n0x%x\n\n",
	//	*(DWORD*)((DWORD)peb->ProcessHeap + 0x40),
	//	*(DWORD*)((DWORD)peb->ProcessHeap + 0x44));

	for (int offset = 0; offset < num; offset++)
	{
		printf("\t[%d] 0x%x 0x%x\n", offset,
			*(LONG*)(((LONG*)peb->ProcessHeaps)[offset] + 0x40),
			*(LONG*)(((LONG*)peb->ProcessHeaps)[offset] + 0x44));
	}


#endif _WIN64

}

// 检查peb标志位
void _IsDebuggerPresent()
{
	BOOL	result;

	result = IsDebuggerPresent();

	OUTMESSAGE(__FUNCTION__, result);
}

// 确定指定的进程是否正在被调试（检测ProcessDebugPort）。
void _CheckRemoteDebuggerPresent()
{
	BOOL	result;

	CheckRemoteDebuggerPresent(NtCurrentProcess,
		&result);

	OUTMESSAGE(__FUNCTION__, result);
}

// 查询当前系统是否被内核调试
void _NtQuerySystemInformation()
{
	BOOL	result;
	NTSTATUS	status;
	SYSTEM_KERNEL_DEBUGGER_INFORMATION	pKerlDbgInfo;
	ULONG		returnLen;

	status = NtQuerySystemInformation(SystemKernelDebuggerInformation,
		&pKerlDbgInfo,
		sizeof(pKerlDbgInfo),
		&returnLen);
	if (NT_SUCCESS(status))
	{
		OUTMESSAGE("SystemKernelDebuggerInformation", !pKerlDbgInfo.KernelDebuggerNotPresent);
	}

}

/*
原理，NtClose函数在释放无效句柄时。如果没有被调试，那么函数返回FALSE。
如果处于调试状态，则会抛出异常C0000008H。
*/
void _NtClose()
{
	BOOL	result = FALSE;

	__try {
		NtClose(ULongToHandle(0x1234));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		result = TRUE;
	}
	OUTMESSAGE(__FUNCTION__, result);
}

/*
进内核查询 ProcessDebugPort、ProcessDebugObjectHandle、ProcessDebugFlags
*/
void _NtQueryInformationProcess()
{
	NTSTATUS	status;
	ULONG		returnlen;

	PVOID		pInfo;

	status = NtQueryInformationProcess(NtCurrentProcess,
		ProcessDebugPort,
		&pInfo,
		sizeof(pInfo),
		&returnlen);
	if (NT_SUCCESS(status))
	{
		OUTMESSAGE("ProcessDebugPort", pInfo != NULL);
	}


	status = NtQueryInformationProcess(NtCurrentProcess,
		ProcessDebugObjectHandle,
		&pInfo,
		sizeof(pInfo),
		&returnlen);
	if (status == 0xC0000353)
	{
		OUTMESSAGE("ProcessDebugObjectHandle", false);
	}
	else
	{
		OUTMESSAGE("ProcessDebugObjectHandle", true);
	}

	status = NtQueryInformationProcess(NtCurrentProcess,
		ProcessDebugFlags,
		&pInfo,
		sizeof(pInfo),
		&returnlen);
	if (NT_SUCCESS(status))
	{
		OUTMESSAGE("ProcessDebugFlags", (ULONG_PTR)pInfo != 1);
	}
	else {
		printf("Query ProcessDebugFlags	fail\n");
	}

}

// 在调试器中隐藏当前线程，断点失效
void _ThreadHideFromDebugger()
{
	NTSTATUS status = NtSetInformationThread(GetCurrentThread(),
		ThreadHideFromDebugger,
		NULL,
		NULL);
	if (NT_SUCCESS(status))
	{
		printf("Thread separate from Debugger successfully\n");
	}
	else {
		printf("Thread separate from Debugger fail.\n");

	}
}


/*
复制后并关闭受保护的句柄 触发异常

调试器对异常处理的影响
	有调试器时：
		调试器优先捕获异常（如 Visual Studio 默认拦截所有异常）。
		如果调试器选择“继续执行”，异常会传递给应用程序的 SEH 处理。
	无调试器时：
		SEH 不能捕获所有类型的异常或错误,
		系统可能直接返回错误码（而非触发异常），导致 SEH 未被触发。
		此时即使操作失败，也不会进入 __except 块。
*/
void _NtDuplicateObject()
{
	BOOL		result = FALSE;
	NTSTATUS	status;

	// 获取当前进程句柄
	HANDLE	hObj = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());

	OBJECT_HANDLE_FLAG_INFORMATION objInfo;
	objInfo.Inherit = false;           // 禁止句柄继承
	objInfo.ProtectFromClose = true;   // 防止句柄被关闭
	status = NtSetInformationObject(
		hObj,                          // 目标句柄
		ObjectHandleFlagInformation,   // 设置句柄标志
		&objInfo,                      // 包含保护标志的结构体
		sizeof(objInfo)                // 结构体大小
	);

	__try {
		status = NtDuplicateObject(
			NtCurrentProcess,  // 源进程（当前进程）
			hObj,             // 源句柄
			NtCurrentProcess,  // 目标进程（当前进程）
			&hObj,            // 返回的新句柄（可以相同）
			NULL,             // 不指定访问权限
			NULL,             // 不指定属性
			DUPLICATE_CLOSE_SOURCE  // 复制后关闭源句柄
		);
	}
#ifdef _WIN64 // EXCEPTION_INVALID_HANDLE 0xC0000008 x64
	__except (GetExceptionCode() == EXCEPTION_INVALID_HANDLE
		? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
#else	// STATUS_HANDLE_NOT_CLOSABLE 0xC0000235 x86返回nt状态码
	__except (GetExceptionCode() == STATUS_HANDLE_NOT_CLOSABLE
		? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
#endif _WIN64
	{
		result = TRUE;
	}
	OUTMESSAGE(__FUNCTION__, result);

}

/*
查询当前系统是否存在调式对象，
*/
void _NtQueryObject()
{
	NTSTATUS	status;
	ULONG		returnLen;
	PCHAR		pBuf;
	POBJECT_TYPES_INFORMATION	pObjTypes;
	POBJECT_TYPE_INFORMATION	pObj;

	WCHAR		szDebugObject[255] = { 0 };
	wcscpy_s(szDebugObject, L"DebugObject");

	status = NtQueryObject(NtCurrentProcess,
		ObjectTypesInformation,
		&returnLen,
		sizeof(returnLen),
		&returnLen);

	pBuf = (PCHAR)calloc(1, returnLen);

	status = NtQueryObject(NtCurrentProcess,
		ObjectTypesInformation,
		pBuf,
		returnLen,
		&returnLen);

	pObjTypes = (POBJECT_TYPES_INFORMATION)pBuf;

	for (int i = 0; i < returnLen - (wcslen(szDebugObject) * sizeof(WCHAR)); i++)
	{
		if (memcmp(szDebugObject, pBuf, wcslen(szDebugObject) * sizeof(WCHAR)) == 0)
		{
			break;
		}
		pBuf++;
	}
	pBuf -= sizeof(ULONG);

	pObj = CONTAINING_RECORD(pBuf, OBJECT_TYPE_INFORMATION, DefaultNonPagedPoolCharge);

	if (pObj->TotalNumberOfHandles || pObj->TotalNumberOfObjects)
	{
		OUTMESSAGE(__FUNCTION__, true);
	}
	else
	{
		OUTMESSAGE(__FUNCTION__, false);
	}

}
