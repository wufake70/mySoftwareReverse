#include "Tool.h"

VOID ForceEnumProcess(PCHAR WhiteList[], PEPROCESS TargetProcess) {
	// 校验 TargetProcess

	PEPROCESS pProcess = NULL;
	PEPROCESS pTempProcess = NULL;
	for (ULONG_PTR pid = 4; pid < 0x100000; pid += 4) {
		pTempProcess = NULL;
		// 通过id获取 EPROCESS结构
		NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &pTempProcess);

		if (!NT_SUCCESS(status)) continue;
		// 判断该结构对应的进程是否退出
		if (PsGetProcessExitStatus(pTempProcess) != STATUS_PENDING) {
			ObDereferenceObject(pTempProcess);
			continue;
		}
		pProcess = pTempProcess;

		// 获取该进程的全路径
		PUNICODE_STRING pszFullPath = NULL;
		status = SeLocateProcessImageName(pTempProcess, &pszFullPath);
		if (!NT_SUCCESS(status)) {
			ObDereferenceObject(pTempProcess);
			continue;
		}

		if (pszFullPath->Buffer == NULL) {
			ObDereferenceObject(pTempProcess);
			continue;
		}
		// 从右边开始匹配镜像名
		PWCH szName = wcsrchr(pszFullPath->Buffer, L'\\');
		if (szName) {
			szName += 1;
			// 是否在白名单内
			for (int j = 0; j < 0x100; j++) {
				if (WhiteList[j] == NULL) break;
				if (_wcsicmp(szName, WhiteList[j]) == 0) {
					pProcess = NULL;
					KdPrintEx((77, 0, "%S in the WhiteList\n", szName));
					break;
				}

			}
		}
		ExFreePool(pszFullPath);

		// 遍历私有句柄表，是否存在被保护进程句柄
		if (pProcess) {
			// +0xf4 私有句柄表
			// 法一：使用未文档化api
			//ExEnumHandleTable(*(ULONG_PTR*)((PUCHAR)pProcess + 0xf4), EnumHandleProcedure, TargetProcess, NULL);

			// 法二：自己遍历私有句柄表
			SetHandleAccess(TargetProcess, pProcess);
			pProcess = NULL;
		}
		ObDereferenceObject(pTempProcess);

	}
}

// handle down grade
BOOLEAN NTAPI EnumHandleProcedure(PHANDLE_TABLE_ENTRY pHandleTableEntry, HANDLE Handle, PVOID TagetProcess) {
	// 去除属性位
	POBJECT_HEADER pObjectHeader = (POBJECT_HEADER)((ULONG)pHandleTableEntry->Object & 0xfffffff8);
	if (MmIsAddressValid(pObjectHeader) == FALSE) return FALSE;
	// 进程类型的索引为 0x7，是否存在目标进程
#ifdef _WIN64
	if (pObjectHeader->TypeIndex == 0x7 && ((PUCHAR)pObjectHeader + 0x30) == TagetProcess) {
#else
	if (pObjectHeader->TypeIndex == 0x7 && ((PUCHAR)pObjectHeader + 0x18) == TagetProcess) {
#endif // _WIN64

		KdPrintEx((77, 0, "TagetProcess is opened\n"));
		pHandleTableEntry->GrantedAccess &= ~(PROCESS_VM_READ | PROCESS_VM_WRITE);
		return TRUE;
	}
	return FALSE;
}

PEPROCESS FindProcessByNameW(PWCH processName) {
	PEPROCESS pProcess = NULL;
	for (ULONG_PTR pid = 12; pid < 0x100000; pid += 4) {
		PEPROCESS pTempProcess = NULL;
		// 通过id获取 EPROCESS结构
		NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &pTempProcess);

		if (!NT_SUCCESS(status)) continue;
		// 判断该结构对应的进程是否退出
		if (PsGetProcessExitStatus(pTempProcess) != STATUS_PENDING) {
			ObDereferenceObject(pTempProcess);
			continue;
		}

		// 获取该进程的全路径
		PUNICODE_STRING szFullPath = NULL;
		status = SeLocateProcessImageName(pTempProcess, &szFullPath);
		if (!NT_SUCCESS(status)) {
			ObDereferenceObject(pTempProcess);
			continue;
		}

		// 从右边开始匹配镜像名
		PWCH szName = wcsrchr(szFullPath->Buffer, L'\\');
		if (szName) {
			szName += 1;
			if (_wcsicmp(szName, processName) == 0) {
				pProcess = pTempProcess;
			}
		}
		ExFreePool(szFullPath);
		ObDereferenceObject(pTempProcess);
		if (pProcess) break;

	}
	return pProcess;
}


PEPROCESS FindProcessByNameA(char* processName) {
	ANSI_STRING aName;
	UNICODE_STRING uName;
	RtlInitAnsiString(&aName, processName);
	NTSTATUS status = RtlAnsiStringToUnicodeString(&uName, &aName, TRUE);

	if (!NT_SUCCESS(status)) return NULL;

	PEPROCESS pProcess = FindProcessByNameW(uName.Buffer);
	RtlFreeUnicodeString(&uName);
	return pProcess;
}

VOID SetHandleAccess(PEPROCESS srcProcess, PEPROCESS targetProcess)
{

	ULONG pObjectTable = *(ULONG*)((ULONG)targetProcess + 0xf4);
	ULONG TableCode = *(ULONG*)pObjectTable;
	ULONG TableDeeth = TableCode & 0xF;//拿到最后一位
	ULONG TableCount = 0;
	ULONG TableBaseAddr = 0;
	ULONG TableDirBaseAddr = 0;
	ULONG TableOneDirBaseAddr = 0;
	ULONG Object = 0;

	switch (TableDeeth)
	{
	case 0:	// 句柄表
		TableBaseAddr = TableCode & 0xfffffffc;
		for (int i = 0; i < PAGE_SIZE / (sizeof(ULONG) * 2); i++) {
			Object = ((PHANDLE_TABLE_ENTRY)TableBaseAddr)[i].Object;
			if (Object == NULL) continue;
			if (!MmIsAddressValid(Object)) break;
			if (((POBJECT_HEADER)(Object & 0xfffffff8))->TypeIndex == 0x7)
				if ((Object & 0xfffffff8) + 0x18 == (ULONG)srcProcess) {
					((PHANDLE_TABLE_ENTRY)TableBaseAddr)[i].GrantedAccess &= ~(PROCESS_VM_READ | PROCESS_VM_WRITE);
					break;
				}
		}
		break;
	case 1:		// 句柄目录表 -> 句柄表
		TableDirBaseAddr = TableCode & 0xfffffffc;
		for (int j = 0; j < PAGE_SIZE / sizeof(ULONG); j++) {
			TableBaseAddr = ((ULONG*)TableDirBaseAddr)[j];
			if (TableBaseAddr == NULL)	continue;
			if (!MmIsAddressValid(TableBaseAddr)) break;
			for (int i = 0; i < PAGE_SIZE / (sizeof(ULONG) * 2); i++) {
				Object = ((PHANDLE_TABLE_ENTRY)TableBaseAddr)[i].Object;
				if (Object == NULL) continue;
				if (!MmIsAddressValid(Object)) break;
				if (((POBJECT_HEADER)(Object & 0xfffffff8))->TypeIndex == 0x7)
					if ((Object & 0xfffffff8) + 0x18 == (ULONG)srcProcess) {
						((PHANDLE_TABLE_ENTRY)TableBaseAddr)[i].GrantedAccess &= ~(PROCESS_VM_READ | PROCESS_VM_WRITE);
						break;
					}
			}
		}

		break;
	case 2:		// 句柄目录基址目录 -> 句柄目录表 -> 句柄表
		TableOneDirBaseAddr = TableCode & 0xfffffffc;
		// .....

		break;
	}
}

