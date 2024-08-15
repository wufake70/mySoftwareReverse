#include "ProcessProtect.h"
#include "Search.h"
#include <intrin.h>


// 关闭写保护
ULONG_PTR wpOff()
{
	ULONG_PTR cr0 = __readcr0();
	// 关闭中断
	_disable();
	// cr0的wp位置零，即可关闭写保护
	__writecr0(cr0 & (~0x10000));
	return cr0;
}

// 开启写保护
VOID wpOn(ULONG_PTR value)
{
	__writecr0(value);
	_enable();
}

// 搜索全局句柄表基址
PVOID GetPspCidTable()
{
	static PVOID scidTable = NULL;
	if (scidTable) return scidTable;
	
	UNICODE_STRING unName = {0};
	RtlInitUnicodeString(&unName, L"PsLookupProcessByProcessId");
	// 通过名字 获取地址
	PUCHAR startFunc = MmGetSystemRoutineAddress(&unName);
#ifdef _WIN64
	// 48 8B 0D 89 20 F1 FF     mov     rcx, cs:PspCidTable
	// PspCidTabled地址的计算 下一条指令地址+fff12089(偏移)-0x100000000
	scidTable = (PVOID)searchOtherCode(startFunc, startFunc+0x100,"498bdc488bd1488b0d****e8****",9);
	if (scidTable)
	{
		// 下一条指令地址+fff12089(偏移)-0x100000000 就是PspCidTable，
		scidTable = (ULONG_PTR)scidTable + 4 + *(PULONG)scidTable - 0x100000000; 
		// PspCidTable 一次解引用后就是 全局句柄表地址
		scidTable = *(PVOID ***)scidTable;
	}
#else
	// win7x32
	scidTable = (PVOID)searchOtherCode(startFunc, startFunc+0x100,"8B*****E8****8BF885FF74*8B1F",2);
	if (scidTable)
	{
		// 特征码地址，一次解引用后就是PspCidTable，
		// PspCidTable 一次解引用后就是 全局句柄表地址
		scidTable = **(PVOID ***)scidTable;
	}
#endif

	
	return scidTable;
}

ULONG_PTR GetExpLookupHandleTableEntry()
{
	static PVOID sExpLookupHandleTableEn = NULL;
	if (sExpLookupHandleTableEn) return sExpLookupHandleTableEn;

	UNICODE_STRING unName = { 0 };
	RtlInitUnicodeString(&unName, L"ExEnumHandleTable");
	PUCHAR startFunc = MmGetSystemRoutineAddress(&unName);
#ifdef _WIN64
	PUCHAR searchFunc = (PUCHAR)searchOtherCode(startFunc, startFunc + 0x200, "488BD3498BCDE8****493BC2", 7);
	sExpLookupHandleTableEn = (ULONG_PTR)searchFunc+4+ *(PULONG)searchFunc-0x100000000;
#else
	PUCHAR searchFunc = (PUCHAR)searchOtherCode(startFunc, startFunc + 0x200, "FF75*8B4D*E8****8BF085F675*EB", 6);
	if (searchFunc)
	{
		sExpLookupHandleTableEn = (searchFunc + 5) + *(PULONG)(searchFunc+1);
	}
#endif // __WIN64



	return sExpLookupHandleTableEn;
}


ULONG GetProcessIdOffset()
{
	
	UNICODE_STRING unName = { 0 };
	RtlInitUnicodeString(&unName, L"PsGetProcessId"); 
	PUCHAR startFunc = MmGetSystemRoutineAddress(&unName);
#ifdef _WIN64
	return *(PULONG)searchOtherCode(startFunc, startFunc + 0x200, "488B81****C3", 3);
#else
	// win7 x32
	return *(PULONG)searchOtherCode(startFunc, startFunc + 0x200, "8B45088B80****5DC20400", 5);
#endif // _WIN64

}

PVOID MyExpLookupHandleTableEntry(HANDLE pid)
{
#ifdef _WIN64
	typedef  PVOID (__fastcall *ExpLookupHandleTableEntryProc)(PVOID handleTable,HANDLE id);
#else

	//x64 不需要这么来定义调用堆栈 原因模拟x86 this call 所以用的fastcall来模拟，参数定义为3个
	// ExpLookupHandleTableEntry 本质参数只有2个，第一个通过ecx，第二个通过堆栈
	// __fastcall 前两个参数分别使用 ecx、eax传参，后面的通过堆栈
	typedef  PVOID (__fastcall *ExpLookupHandleTableEntryProc)(PVOID handleTable, ULONG xxx,HANDLE id);
#endif // _WIN64
	ExpLookupHandleTableEntryProc func = (ExpLookupHandleTableEntryProc)GetExpLookupHandleTableEntry();
	PVOID pspTable = GetPspCidTable();
	if (!pspTable || !func) return NULL;
#ifdef _WIN64
	return func(pspTable, pid);
#else
	return func(pspTable,0, pid);
#endif // _WIN64

}

BOOLEAN ProtectProcess(HANDLE pid)
{
	PEPROCESS Process = NULL;
	NTSTATUS status = PsLookupProcessByProcessId(pid, &Process);
	if (!NT_SUCCESS(status)) return FALSE;

	ObDereferenceObject(Process);
	
	PVOID hanldeEntry = MyExpLookupHandleTableEntry(pid);
	// 抹除目标进程句柄
#ifdef _WIN64
	memset(hanldeEntry, 0, 0x10);
#else
	memset(hanldeEntry, 0, 8);
#endif // _WIN64

	ULONG offset = GetProcessIdOffset();
	// 目标进程结构的 pid成员赋值为0，避免进程退出时蓝屏
	*(PHANDLE)((PUCHAR)Process + offset) = 0;

	return TRUE;
}