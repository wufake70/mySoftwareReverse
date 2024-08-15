#include "ProcessProtect.h"
#include "Search.h"
#include <intrin.h>


// �ر�д����
ULONG_PTR wpOff()
{
	ULONG_PTR cr0 = __readcr0();
	// �ر��ж�
	_disable();
	// cr0��wpλ���㣬���ɹر�д����
	__writecr0(cr0 & (~0x10000));
	return cr0;
}

// ����д����
VOID wpOn(ULONG_PTR value)
{
	__writecr0(value);
	_enable();
}

// ����ȫ�־�����ַ
PVOID GetPspCidTable()
{
	static PVOID scidTable = NULL;
	if (scidTable) return scidTable;
	
	UNICODE_STRING unName = {0};
	RtlInitUnicodeString(&unName, L"PsLookupProcessByProcessId");
	// ͨ������ ��ȡ��ַ
	PUCHAR startFunc = MmGetSystemRoutineAddress(&unName);
#ifdef _WIN64
	// 48 8B 0D 89 20 F1 FF     mov     rcx, cs:PspCidTable
	// PspCidTabled��ַ�ļ��� ��һ��ָ���ַ+fff12089(ƫ��)-0x100000000
	scidTable = (PVOID)searchOtherCode(startFunc, startFunc+0x100,"498bdc488bd1488b0d****e8****",9);
	if (scidTable)
	{
		// ��һ��ָ���ַ+fff12089(ƫ��)-0x100000000 ����PspCidTable��
		scidTable = (ULONG_PTR)scidTable + 4 + *(PULONG)scidTable - 0x100000000; 
		// PspCidTable һ�ν����ú���� ȫ�־�����ַ
		scidTable = *(PVOID ***)scidTable;
	}
#else
	// win7x32
	scidTable = (PVOID)searchOtherCode(startFunc, startFunc+0x100,"8B*****E8****8BF885FF74*8B1F",2);
	if (scidTable)
	{
		// �������ַ��һ�ν����ú����PspCidTable��
		// PspCidTable һ�ν����ú���� ȫ�־�����ַ
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

	//x64 ����Ҫ��ô��������ö�ջ ԭ��ģ��x86 this call �����õ�fastcall��ģ�⣬��������Ϊ3��
	// ExpLookupHandleTableEntry ���ʲ���ֻ��2������һ��ͨ��ecx���ڶ���ͨ����ջ
	// __fastcall ǰ���������ֱ�ʹ�� ecx��eax���Σ������ͨ����ջ
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
	// Ĩ��Ŀ����̾��
#ifdef _WIN64
	memset(hanldeEntry, 0, 0x10);
#else
	memset(hanldeEntry, 0, 8);
#endif // _WIN64

	ULONG offset = GetProcessIdOffset();
	// Ŀ����̽ṹ�� pid��Ա��ֵΪ0����������˳�ʱ����
	*(PHANDLE)((PUCHAR)Process + offset) = 0;

	return TRUE;
}