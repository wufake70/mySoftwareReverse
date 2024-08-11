#include <ntifs.h>
#include <intrin.h>

// �ر�д����
ULONG wpOff()
{
	ULONG cr0 = __readcr0();
	// �ر��ж�
	_disable();
	// cr0��wpλ���㣬���ɹر�д����
	__writecr0(cr0 & (~0x10000));
	return cr0;
}

// ����д����
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
	POBJECT_TYPE pObjectType; // ���̶�������PsProcessType
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

// ȫ�ֱ���
PVOID RegistrationHandle = NULL;
PUCHAR jmpBoard = 0;

VOID DriverUnload(PDRIVER_OBJECT pDrivr);

OB_PREOP_CALLBACK_STATUS ProcessPreCallBack(
	_In_ PVOID RegistrationContext,
	_Inout_ POB_PRE_OPERATION_INFORMATION OperationInformation
);

VOID __declspec(naked)ProxyJmpBoardfunc() {
	__asm {
		pop ebx; // ��ջƽ��
		pop ebx;
		pop eax;
		jmp ProcessPreCallBack; // �ص�����
	}
}

PUCHAR GetSignatureCodeAddress(PUCHAR imageBase,ULONG_PTR imageSize,PUCHAR SignatureCode,USHORT SignatureCodeSize);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg) {
	pDriver->DriverUnload = DriverUnload;


	PDRIVER_OBJECT ObNullDriver = NULL;
	PLDR_DATA_TABLE_ENTRY pLdr = NULL;
	UNICODE_STRING DriverName = { 0 };

	// ���� NULL����
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
		*(PUCHAR)&jmpBoard[2] = 0xE8; // call ,αװ �ص�����
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

	obCallBackInfoReg.Version = ObGetFilterVersion();	// ʹ��api��ȡ
	obCallBackInfoReg.OperationRegistrationCount = 1; // �ص���������
	obCallBackInfoReg.RegistrationContext = "hhhhh";	// ����
	RtlInitUnicodeString(&obCallBackInfoReg.Altitude, L"12345");	// ���ȼ�
	obCallBackInfoReg.OperationRegistration = &obOperationInfoReg;

	obOperationInfoReg.ObjectType = *PsProcessType; // ��������
	// ���������dump
	obOperationInfoReg.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	// �������ǰ�ص������Թ��˾��Ȩ��
	obOperationInfoReg.PreOperation = ProcessPreCallBack;
	//obOperationInfoReg.PreOperation = jmpBoard;
	// ���������ص�
	obOperationInfoReg.PostOperation = NULL;

	// **�޸ı�־λ���Ա�ͨ���ص���֤
	((PLDR_DATA_TABLE_ENTRY)pDriver->DriverSection)->Flags |= 0x20;
	status = ObRegisterCallbacks(&obCallBackInfoReg, &RegistrationHandle);
	if (NT_SUCCESS(status)) {
		KdPrintEx((77, 0, "ObRegisterCallbacks OK\n"));
		// ������
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
	// ��ԭ jmpBoard
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
	PEPROCESS pEprocess = IoGetCurrentProcess();	// ��ǰ����
	PEPROCESS TargetProcess = OperationInformation->Object;	// �½���

	PUCHAR currName = PsGetProcessImageFileName(pEprocess);
	PUCHAR targetName = PsGetProcessImageFileName(TargetProcess);

	if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) {
		KdPrintEx((77, 0, "open %s->%s context:%s\n", currName, targetName, RegistrationContext));

		// ���˾��(�¾��������ǰ)���޸���Ȩ��λ ���Դﵽ���ƶ�д��
		// ��Ҫ���ð����������� explorer �޷�����Ŀ�����
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

	return OB_PREOP_SUCCESS; // ֻ�ܷ������ֵ
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

