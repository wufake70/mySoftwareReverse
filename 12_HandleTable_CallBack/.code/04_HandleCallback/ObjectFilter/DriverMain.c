#include <ntifs.h>

EXTERN_C PUCHAR PsGetProcessImageFileName(PEPROCESS pEprocess);

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
	PUCHAR LoadCount;                                                       //0x38
	PUCHAR TlsIndex;                                                        //0x3a
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

// 全局变量
PVOID RegistrationHandle = NULL;

VOID DriverUnload(PDRIVER_OBJECT pDrivr);

OB_PREOP_CALLBACK_STATUS ProcessPreCallBack(
	_In_ PVOID RegistrationContext,
	_Inout_ POB_PRE_OPERATION_INFORMATION OperationInformation
	);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver,PUNICODE_STRING pReg) {
	pDriver->DriverUnload = DriverUnload;

	OB_CALLBACK_REGISTRATION obRegInfo = { 0 };
	OB_OPERATION_REGISTRATION obCallBackInfo = { 0 };

	obRegInfo.Version = ObGetFilterVersion();	// 使用api获取
	obRegInfo.OperationRegistrationCount = 1; // 回调函数个数
	obRegInfo.RegistrationContext = "hhhhh";	// 参数
	RtlInitUnicodeString(&obRegInfo.Altitude,L"12345");	// 优先级，越小越高，(过检测时，要高于游戏检测)
	obRegInfo.OperationRegistration = &obCallBackInfo;

	obCallBackInfo.ObjectType = *PsProcessType; // 对象类型
	// 创建句柄、dump
	obCallBackInfo.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	// 创建句柄前回调，可以过滤句柄权限
	obCallBackInfo.PreOperation = ProcessPreCallBack; // 注册时，会校验回调函数
	// 创建句柄后回调
	obCallBackInfo.PostOperation = NULL;

	// **修改回调权限位
	((PLDR_DATA_TABLE_ENTRY)pDriver->DriverSection)->Flags |= 0x20;
	NTSTATUS status = ObRegisterCallbacks(&obRegInfo, &RegistrationHandle);
	//KdBreakPoint();

	return STATUS_SUCCESS;

}

VOID DriverUnload(PDRIVER_OBJECT pDrivr) {
	if(RegistrationHandle) ObUnRegisterCallbacks(RegistrationHandle);
	KdPrintEx((77,0,"DriverUnload\n"));
}

OB_PREOP_CALLBACK_STATUS ProcessPreCallBack(
	_In_ PVOID RegistrationContext,
	_Inout_ POB_PRE_OPERATION_INFORMATION OperationInformation
) {
	PEPROCESS pEprocess = IoGetCurrentProcess();	// 当前进程
	PEPROCESS TargetProcess = OperationInformation->Object;	// 新进程

	PUCHAR currName = PsGetProcessImageFileName(pEprocess);
	PUCHAR targetName = PsGetProcessImageFileName(TargetProcess);

	if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) {
		KdPrintEx((77, 0, "open %s->%s context:%s\n", currName, targetName,RegistrationContext));

		// 过滤句柄(新句柄被插入前)，修改其权限位 可以达到限制读写，
		// 需要设置白名单，否则 explorer 无法启动目标程序
		if (_stricmp(targetName, "qq.exe")==0) {	
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
		}
	}
	else
	{
		KdPrintEx((77, 0, "dump %s->%s context:%s\n", currName, targetName, RegistrationContext));

		if (_stricmp(targetName, "qq.exe") == 0) {
			OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = 0;
		}
	}

	return OB_PREOP_SUCCESS; // 只能返回这个值
}