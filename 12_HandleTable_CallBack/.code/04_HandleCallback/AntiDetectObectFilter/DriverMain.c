#include <ntifs.h>
#include <intrin.h>

// 关闭写保护
ULONG wpOff()
{
	ULONG cr0 = __readcr0();
	// 关闭中断
	_disable();
	// cr0的wp位置零，即可关闭写保护
	__writecr0(cr0 & (~0x10000));
	return cr0;
}

// 开启写保护
VOID wpOn(ULONG value)
{
	__writecr0(value);
	_enable();
}

EXTERN_C PUCHAR PsGetProcessImageFileName(PEPROCESS pEprocess);

EXTERN_C NTSTATUS ObReferenceObjectByName(
	__in PUNICODE_STRING ObjectName,
	__in ULONG Attributes,
	__in_opt PACCESS_STATE AccessState,
	__in_opt ACCESS_MASK DesiredAccess,
	__in POBJECT_TYPE ObjectType,
	__in KPROCESSOR_MODE AccessMode,
	__inout_opt PVOID ParseContext,
	__out PVOID *Object);

EXTERN_C POBJECT_TYPE *IoDriverObjectType;

typedef struct _OB_OPERATION_REG_LIST_ENTRY {
	LIST_ENTRY callBackLIstEntry;
	UINT32 operations;
	UINT32 flags;
	UINT32 self;
	POBJECT_TYPE pObjectType; // 进程对象类型PsProcessType
	PUCHAR PreOperation;
	PUCHAR PostOperation;
	UINT32 field;
}OB_OPERATION_REG_LIST_ENTRY,*POB_OPERATION_REG_LIST_ENTRY;

typedef struct _OB_HANDLE {
	UINT32 version;
	PVOID RegistrationContext;
	UINT32 operationRegistrationCount;
	PUCHAR Altitude;
	OB_OPERATION_REG_LIST_ENTRY ObOperation;
}OB_HANDLE,*POB_HANDLE;


//0x88 bytes (sizeof)
typedef struct _OBJECT_TYPE
{
	struct _LIST_ENTRY TypeList;                                            //0x0
	struct _UNICODE_STRING Name;                                            //0x8
	VOID* DefaultObject;                                                    //0x10
	UCHAR Index;                                                            //0x14
	ULONG TotalNumberOfObjects;                                             //0x18
	ULONG TotalNumberOfHandles;                                             //0x1c
	ULONG HighWaterNumberOfObjects;                                         //0x20
	ULONG HighWaterNumberOfHandles;                                         //0x24
	UINT32 TypeInfo;                               //0x28
	UINT32 TypeLock;                                          //0x78
	ULONG Key;                                                              //0x7c
	struct _LIST_ENTRY CallbackList;                                        //0x80
}OBJECT_TYPE,*POBJECT_TYPE;

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
}LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

// 全局变量
PVOID RegistrationHandle = NULL;
PUCHAR jmpBoard = 0;

VOID DriverUnload(PDRIVER_OBJECT pDrivr);

OB_PREOP_CALLBACK_STATUS ProcessPreCallBack(
	_In_ PVOID RegistrationContext,
	_Inout_ POB_PRE_OPERATION_INFORMATION OperationInformation
);

VOID __declspec(naked)ProxyJmpBoardfunc() {
	__asm {
		pop ebx; // 堆栈平衡
		pop ebx;
		pop eax;
		jmp ProcessPreCallBack; // 回调函数
	}
}

PUCHAR GetSignatureCodeAddress(PUCHAR imageBase,ULONG_PTR imageSize,PUCHAR SignatureCode,USHORT SignatureCodeSize);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg) {
	pDriver->DriverUnload = DriverUnload;


	PDRIVER_OBJECT ObNullDriver = NULL;
	PLDR_DATA_TABLE_ENTRY pLdr = NULL;
	UNICODE_STRING DriverName = { 0 };

	// 搜索 NULL驱动
	RtlInitUnicodeString(&DriverName, L"\\driver\\null");

	NTSTATUS status = ObReferenceObjectByName(&DriverName, FILE_ALL_ACCESS, 0, 0, *IoDriverObjectType, KernelMode, NULL, &ObNullDriver);
	if (!NT_SUCCESS(status)) {
		KdPrintEx((77, 0, "get null.sys fail\n"));
		return 0;
	}
	pLdr = (PLDR_DATA_TABLE_ENTRY)ObNullDriver->DriverSection;
	KdPrintEx((77, 0, "null.sys base:%x, size:%x", pLdr->DllBase, pLdr->SizeOfImage));

	jmpBoard = GetSignatureCodeAddress(pLdr->DllBase, pLdr->SizeOfImage, "This pr",7);

	KdPrintEx((77, 0, "jmpBoard: %x\n", jmpBoard));
	//jmpBoard = 0x9284004f;
	if (MmIsAddressValid(jmpBoard)) {
		UINT32 value = wpOff();
		*(PUCHAR)&jmpBoard[0] = 0x50; // push eax
		*(PUCHAR)&jmpBoard[1] = 0x53; // push ebx
		*(PUCHAR)&jmpBoard[2] = 0xE8; // call ,伪装 回调函数
		*(PULONG)&jmpBoard[3] = (ULONG)ProxyJmpBoardfunc -(ULONG)&jmpBoard[2]-5;
		wpOn(value);
		KdPrintEx((77, 00, "set jmpBoard successful\n"));

		goto RegisterHandleFilterCallBack;
	}
	else {
		KdPrintEx((77, 0, "set jmpBoard false\n"));
		goto RegisterHandleFilterCallBackEnd;
	}
	OB_CALLBACK_REGISTRATION obCallBackInfoReg = { 0 };
	OB_OPERATION_REGISTRATION obOperationInfoReg = { 0 };
RegisterHandleFilterCallBack:

	obCallBackInfoReg.Version = ObGetFilterVersion();	// 使用api获取
	obCallBackInfoReg.OperationRegistrationCount = 1; // 回调函数个数
	obCallBackInfoReg.RegistrationContext = "hhhhh";	// 参数
	RtlInitUnicodeString(&obCallBackInfoReg.Altitude, L"12345");	// 优先级
	obCallBackInfoReg.OperationRegistration = &obOperationInfoReg;

	obOperationInfoReg.ObjectType = *PsProcessType; // 对象类型
	// 创建句柄、dump
	obOperationInfoReg.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	// 创建句柄前回调，可以过滤句柄权限
	obOperationInfoReg.PreOperation = ProcessPreCallBack;
	//obOperationInfoReg.PreOperation = jmpBoard;
	// 创建句柄后回调
	obOperationInfoReg.PostOperation = NULL;

	// **修改标志位，以便通过回调验证
	((PLDR_DATA_TABLE_ENTRY)pDriver->DriverSection)->Flags |= 0x20;
	status = ObRegisterCallbacks(&obCallBackInfoReg, &RegistrationHandle);
	if (NT_SUCCESS(status)) {
		KdPrintEx((77, 0, "ObRegisterCallbacks OK\n"));
		// 加跳板
		UINT32 value = wpOff();
		((POB_HANDLE)RegistrationHandle)->ObOperation.PreOperation = jmpBoard;
		wpOn(value);
	}
	else
	{
		KdPrintEx((77, 0, "ObRegisterCallbacks false,code: 0x%x\n",status));

	}


RegisterHandleFilterCallBackEnd:

	return STATUS_SUCCESS;

}

VOID DriverUnload(PDRIVER_OBJECT pDrivr) {
	// 还原 jmpBoard
	UINT32 value = wpOff();
	PUCHAR text = "This pr";
	for (UINT32 i = 0; i < 7; i++) {
		jmpBoard[i] = text[i];
	}
	wpOn(value);
	if (RegistrationHandle) ObUnRegisterCallbacks(RegistrationHandle);
	KdPrintEx((77, 0, "DriverUnload\n"));
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
		KdPrintEx((77, 0, "open %s->%s context:%s\n", currName, targetName, RegistrationContext));

		// 过滤句柄(新句柄被插入前)，修改其权限位 可以达到限制读写，
		// 需要设置白名单，否则 explorer 无法启动目标程序
		if (_stricmp(targetName, "qq.exe") == 0) {
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

PUCHAR GetSignatureCodeAddress(PUCHAR imageBase, ULONG_PTR imageSize, PUCHAR SignatureCode,USHORT SignatureCodeSize) {
	ULONG i = 0;
	ULONG j = 0;
	PUCHAR SignatureCodeAddress = 0;
	while (i <= imageSize) {
		while (MmIsAddressValid(imageBase + i)&&j < SignatureCodeSize && imageBase[i++] == SignatureCode[j]) {
			//KdPrintEx((77, 0, "%c\n", SignatureCode[j]));
			j++;
		}
		if (MmIsAddressValid(imageBase + i) == FALSE) break;

		if (j == SignatureCodeSize) {
			SignatureCodeAddress = &imageBase[i] - SignatureCodeSize;
			break;
		}
		else {
			j = 0;
		}
	}

	return SignatureCodeAddress;
}

