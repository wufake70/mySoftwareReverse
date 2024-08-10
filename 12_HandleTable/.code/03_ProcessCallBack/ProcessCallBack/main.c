#include <ntifs.h>

//0x78 bytes (sizeof)
typedef struct _LDR_DATA_TABLE_ENTRY
{
	struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
	struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x8
	struct _LIST_ENTRY InInitializationOrderLinks;                          //0x10
	VOID* DllBase;                                                          //0x18
	VOID* EntryPoint;                                                       //0x1c
	ULONG SizeOfImage;                                                      //0x20
	struct _UNICODE_STRING FullDllName;                                     //0x24
	struct _UNICODE_STRING BaseDllName;                                     //0x2c
	ULONG Flags;                                                            //0x34
	USHORT LoadCount;                                                       //0x38
	USHORT TlsIndex;                                                        //0x3a
	union
	{
		struct _LIST_ENTRY HashLinks;                                       //0x3c
		struct
		{
			VOID* SectionPointer;                                           //0x3c
			ULONG CheckSum;                                                 //0x40
		};
	};
	union
	{
		ULONG TimeDateStamp;                                                //0x44
		VOID* LoadedImports;                                                //0x44
	};
	struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x48
	VOID* PatchInformation;                                                 //0x4c
	struct _LIST_ENTRY ForwarderLinks;                                      //0x50
	struct _LIST_ENTRY ServiceTagLinks;                                     //0x58
	struct _LIST_ENTRY StaticLinks;                                         //0x60
	VOID* ContextInformation;                                               //0x68
	ULONG OriginalBase;                                                     //0x6c
	union _LARGE_INTEGER LoadTime;                                          //0x70
}LDR_DATA_TABLE_ENTRY,*PLDR_DATA_TABLE_ENTRY;

VOID DriverUnload(PDRIVER_OBJECT pDriver);
VOID CreateProcessCallBack(
	_In_ HANDLE ParentId,
	_In_ HANDLE ProcessId,
	_In_ BOOLEAN Create);

VOID CreateProcessCallBackEx(
	_Inout_ PEPROCESS Process,
	_In_ HANDLE ProcessId,
	_Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver,PUNICODE_STRING pReg) {
	pDriver->DriverUnload = DriverUnload;

	//PsSetCreateProcessNotifyRoutine(CreateProcessCallBack,FALSE);

	// 对应驱动链表上的 flags 属性位 置为1
	((PLDR_DATA_TABLE_ENTRY)pDriver->DriverSection)->Flags |=0x20;
	NTSTATUS status = PsSetCreateProcessNotifyRoutineEx(CreateProcessCallBackEx,FALSE);
	if (NT_SUCCESS(status)) {
		KdPrintEx((77,0,"CreateProcessCallBackEx register ok\n"));
	}
	else
	{
		//0xC0000022
		//状态_访问_被拒绝
		KdPrintEx((77,0,"CreateProcessCallBackEx register fail\nCode: %x",status));
	}

	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT pDriver) {
	//PsSetCreateProcessNotifyRoutine(CreateProcessCallBack,TRUE);
	// 移除回调函数
	PsSetCreateProcessNotifyRoutineEx(CreateProcessCallBackEx,TRUE);

	KdPrintEx((77,0,"DriverUnload\n"));
}

VOID CreateProcessCallBack(
	_In_ HANDLE ParentId,
	_In_ HANDLE ProcessId,
	_In_ BOOLEAN Create) {
	PEPROCESS pEprocess = NULL;
	PEPROCESS pEprocess_ = NULL;
	if (Create) {
		NTSTATUS status = PsLookupProcessByProcessId(ParentId, &pEprocess);
		if (status);
		ObDereferenceObject(pEprocess);
		status = PsLookupProcessByProcessId(ProcessId, &pEprocess_);
		if (status);
		ObDereferenceObject(pEprocess_);
		KdPrintEx((77,0,"parent pid:%d(%s),new pid:%d(%s)",
			ParentId,(ULONG_PTR)pEprocess+0x16c,
			ProcessId,(ULONG_PTR)pEprocess_+0x16c));
	}else {
		NTSTATUS status = PsLookupProcessByProcessId(ParentId, &pEprocess);
		if (status);
		status = PsLookupProcessByProcessId(ProcessId, &pEprocess_);
		if (status);
		KdPrintEx((77, 0, "parent pid:%d(%s),close pid:%d(%s)",
			ParentId, (ULONG_PTR)pEprocess + 0x16c,
			ProcessId, (ULONG_PTR)pEprocess_ + 0x16c));
	}
	 
}

VOID CreateProcessCallBackEx(
	_Inout_ PEPROCESS Process,  // 这个参数可以被 先注册的回调函数 修改，使用 id搜索
	_In_ HANDLE ProcessId,		
	_Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo) { // 这个参数可以被 先注册的回调函数 修改，

	if (CreateInfo) {
		KdPrintEx((77,0,"pid:%d(%s),Parent pid:%d(%wZ),Create status:%x\n",
			ProcessId,(ULONG_PTR)Process+0x16c,
			CreateInfo->ParentProcessId, CreateInfo->ImageFileName, CreateInfo->CreationStatus));
	}
	else {
		KdPrintEx((77,0,"pid:%d(%s) die\n",ProcessId, (ULONG_PTR)Process + 0x16c));
	}
}