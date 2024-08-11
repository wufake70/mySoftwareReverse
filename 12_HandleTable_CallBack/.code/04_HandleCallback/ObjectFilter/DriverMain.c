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

// ȫ�ֱ���
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

	obRegInfo.Version = ObGetFilterVersion();	// ʹ��api��ȡ
	obRegInfo.OperationRegistrationCount = 1; // �ص���������
	obRegInfo.RegistrationContext = "hhhhh";	// ����
	RtlInitUnicodeString(&obRegInfo.Altitude,L"12345");	// ���ȼ���ԽСԽ�ߣ�(�����ʱ��Ҫ������Ϸ���)
	obRegInfo.OperationRegistration = &obCallBackInfo;

	obCallBackInfo.ObjectType = *PsProcessType; // ��������
	// ���������dump
	obCallBackInfo.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	// �������ǰ�ص������Թ��˾��Ȩ��
	obCallBackInfo.PreOperation = ProcessPreCallBack; // ע��ʱ����У��ص�����
	// ���������ص�
	obCallBackInfo.PostOperation = NULL;

	// **�޸Ļص�Ȩ��λ
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
	PEPROCESS pEprocess = IoGetCurrentProcess();	// ��ǰ����
	PEPROCESS TargetProcess = OperationInformation->Object;	// �½���

	PUCHAR currName = PsGetProcessImageFileName(pEprocess);
	PUCHAR targetName = PsGetProcessImageFileName(TargetProcess);

	if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) {
		KdPrintEx((77, 0, "open %s->%s context:%s\n", currName, targetName,RegistrationContext));

		// ���˾��(�¾��������ǰ)���޸���Ȩ��λ ���Դﵽ���ƶ�д��
		// ��Ҫ���ð����������� explorer �޷�����Ŀ�����
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

	return OB_PREOP_SUCCESS; // ֻ�ܷ������ֵ
}