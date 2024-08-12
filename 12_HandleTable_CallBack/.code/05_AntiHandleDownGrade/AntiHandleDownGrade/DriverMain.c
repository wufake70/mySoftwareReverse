#include <ntifs.h>
#include <intrin.h>
#include "Tool.h"

VOID DiverUnload(PDRIVER_OBJECT pDdriver);

// handle down grade
BOOLEAN NTAPI EnumHandleProcedure2(PHANDLE_TABLE_ENTRY pHandleTableEntry, HANDLE Handle, PVOID TagetProcess);
// 还原句柄项
BOOLEAN NTAPI EnumHandleProcedure3(PHANDLE_TABLE_ENTRY pHandleTableEntry, HANDLE Handle, PVOID TagetProcess);


PEPROCESS CEpEprocess = NULL;
PEPROCESS TargetpEprocess = NULL;
PEPROCESS NewpEprocess = NULL;
PUCHAR newPageDirPointerTable = NULL;

NTSTATUS DriverEntry(PDRIVER_OBJECT pDdriver, PUNICODE_STRING pReg) {
	pDdriver->DriverUnload = DiverUnload;

	NewpEprocess = ExAllocatePool(NonPagedPool, 0x2c0+0x18); // 对象头+进程结构对象
	if (NewpEprocess == NULL) {
		KdPrintEx((77, 0, "memory alloc fail\n"));
		return 0;
	}
	
	CEpEprocess = FindProcessByNameA("cheatengine-i386.exe");
	TargetpEprocess = FindProcessByNameA("explorer.exe");
	if (!CEpEprocess) {
		KdPrintEx((77, 0, "get CE process structure fail\n"));
		return 0;
	}
	if (!TargetpEprocess) {
		KdPrintEx((77, 0, "get target process structure fail\n"));
		return 0;
	}

	// 结构复制，从对象头开始
	memcpy(NewpEprocess, (ULONG_PTR)TargetpEprocess-0x18, 0x2c0+0x18);

	// ** 抹除原来进程结构特征
	*(PULONG_PTR)((ULONG_PTR)NewpEprocess + 0x18 + 0xB4) = 3;	// pid
	*(PULONG_PTR)((ULONG_PTR)NewpEprocess + 0x18 + 0xF4) = 0;	// 私有句柄表
	*(PULONG_PTR)((ULONG_PTR)NewpEprocess + 0x18 + 0x16C) = 0;	// image file name
	// 修改cr3
	ULONG_PTR cr3LineAddr = 0;
	PHYSICAL_ADDRESS OldCr3PhysicalAddr = { 0 };
	// 将cr3转换成线性地址
	OldCr3PhysicalAddr.LowPart = *(PULONG_PTR)((ULONG_PTR)NewpEprocess + 0x18 + 0x18);
	KdPrintEx((77, 0, "old cr3: %x\n", OldCr3PhysicalAddr.LowPart));
	cr3LineAddr = MmMapIoSpace(OldCr3PhysicalAddr, 0x1000, MmNonCached); // 获取cr3线性地址、映射一个物理页大小
	if (cr3LineAddr == NULL) return	0;
	// 申请空间复制 页目录指针表
	newPageDirPointerTable = ExAllocatePool(NonPagedPool, 0x1000);
	if (newPageDirPointerTable == NULL) return 0;
	memcpy_s(newPageDirPointerTable, 0x1000, cr3LineAddr, 0x1000);
	// 将新的表的基址 转为 物理地址，并赋给 新的进程结构CR3成员
	OldCr3PhysicalAddr = MmGetPhysicalAddress(newPageDirPointerTable);
	KdPrintEx((77, 0, "new cr3: %x\n", OldCr3PhysicalAddr.LowPart));
	*(PULONG_PTR)((ULONG_PTR)NewpEprocess + 0x18 + 0x18) = OldCr3PhysicalAddr.LowPart;

	// 修改句柄项
	// CE是否处于运行状态
	if(PsGetProcessExitStatus(CEpEprocess) == STATUS_PENDING)
		ExEnumHandleTable(*(ULONG_PTR*)((PUCHAR)CEpEprocess + 0xf4), EnumHandleProcedure2, TargetpEprocess, NULL);
	return STATUS_SUCCESS;
}

VOID DiverUnload(PDRIVER_OBJECT pDdriver) {
	// 还原
	if (MmIsAddressValid((PUCHAR)CEpEprocess + 0x274) && *(PULONG)((PUCHAR)CEpEprocess + 0x274) == 0x103) {

		ExEnumHandleTable(*(ULONG_PTR*)((PUCHAR)CEpEprocess + 0xf4), EnumHandleProcedure3, NewpEprocess, NULL);
	}
	ExFreePool(newPageDirPointerTable);
	ExFreePool(NewpEprocess);
	KdPrintEx((77, 0, "DiverUnload\n"));
}

// handle up grade
BOOLEAN NTAPI EnumHandleProcedure2(PHANDLE_TABLE_ENTRY pHandleTableEntry, HANDLE Handle, PVOID TagetProcess) {
	ULONG AttributeBit = (ULONG)pHandleTableEntry->Object & ~0xfffffff8;
	// 去除属性位
	POBJECT_HEADER pObjectHeader = (POBJECT_HEADER)((ULONG)pHandleTableEntry->Object & 0xfffffff8);
	if (MmIsAddressValid(pObjectHeader) == FALSE) return FALSE;
	// 进程类型的索引为 0x7，是否存在目标进程
	if (pObjectHeader->TypeIndex == 0x7 && ((PUCHAR)pObjectHeader + 0x18) == TagetProcess) {
		KdPrintEx((77, 0, "switch process structure\n"));
		pObjectHeader = NewpEprocess;
		(ULONG_PTR)pObjectHeader |= AttributeBit;
		pHandleTableEntry->Object = pObjectHeader;
		pHandleTableEntry->GrantedAccess |= PROCESS_ALL_ACCESS;
		return TRUE;
	}
	return FALSE;
}
// 还原句柄项
BOOLEAN NTAPI EnumHandleProcedure3(PHANDLE_TABLE_ENTRY pHandleTableEntry, HANDLE Handle, PVOID TagetProcess) {
	UCHAR AttributeBit = (POBJECT_HEADER)((ULONG)pHandleTableEntry->Object & ~0xfffffff8);
	// 去除属性位
	POBJECT_HEADER pObjectHeader = ((ULONG)pHandleTableEntry->Object & 0xfffffff8);
	if (MmIsAddressValid(pObjectHeader) == FALSE) return FALSE;
	// 进程类型的索引为 0x7，是否存在目标进程
	if (pObjectHeader->TypeIndex == 0x7 && ((PUCHAR)pObjectHeader) == TagetProcess) {
		KdPrintEx((77, 0, "switch process structure\n"));
		pHandleTableEntry->GrantedAccess |= PROCESS_ALL_ACCESS;
		pObjectHeader = (ULONG_PTR)TargetpEprocess-0x18;
		(ULONG_PTR)pObjectHeader |= AttributeBit;
		pHandleTableEntry->Object = pObjectHeader;
		return TRUE;
	}
	return FALSE;
}