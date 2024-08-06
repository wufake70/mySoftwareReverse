#include <ntifs.h>

ULONG Pow(ULONG base, ULONG exponent) {
	ULONG result = 1;
	for (ULONG i = 0; i < exponent; i++) {
		result *= base;
	}
	return result;
}

VOID DriverUnload(PDRIVER_OBJECT pDriver) {

}

NTKERNELAPI PVOID NTAPI
ObGetObjectType(
	IN PVOID pObject
);

//0x3c bytes (sizeof)
typedef struct _HANDLE_TABLE
{
	ULONG TableCode;                                                        //0x0
	struct _EPROCESS* QuotaProcess;                                         //0x4
	VOID* UniqueProcessId;                                                  //0x8
	UINT32 HandleLock;                                        //0xc
	struct _LIST_ENTRY HandleTableList;                                     //0x10
	UINT32 HandleContentionEvent;                             //0x18
	struct _HANDLE_TRACE_DEBUG_INFO* DebugInfo;                             //0x1c
	LONG ExtraInfoPages;                                                    //0x20
	union
	{
		ULONG Flags;                                                        //0x24
		UCHAR StrictFIFO : 1;                                                 //0x24
	};
	ULONG FirstFreeHandle;                                                  //0x28
	struct _HANDLE_TABLE_ENTRY* LastFreeHandleEntry;                        //0x2c
	ULONG HandleCount;                                                      //0x30
	ULONG NextHandleNeedingPool;                                            //0x34
	ULONG HandleCountHighWatermark;                                         //0x38
}HANDLE_TABLE,*PHANDLE_TABLE;

//0x8 bytes (sizeof)
typedef struct _HANDLE_TABLE_ENTRY
{
	union
	{
		VOID* Object;                                                       //0x0
		ULONG ObAttributes;                                                 //0x0
		struct _HANDLE_TABLE_ENTRY_INFO* InfoTable;                         //0x0
		ULONG Value;                                                        //0x0
	};
	union
	{
		ULONG GrantedAccess;                                                //0x4
		struct
		{
			USHORT GrantedAccessIndex;                                      //0x4
			USHORT CreatorBackTraceIndex;                                   //0x6
		};
		ULONG NextFreeTableEntry;                                           //0x4
	};
}HANDLE_TABLE_ENTRY,*PHANDLE_TABLE_ENTRY;

//0x20 bytes (sizeof)
typedef struct _OBJECT_HEADER
{
	LONG PointerCount;                                                      //0x0
	union
	{
		LONG HandleCount;                                                   //0x4
		VOID* NextToFree;                                                   //0x4
	};
	UINT32 Lock;                                              //0x8
	UCHAR TypeIndex;                                                        //0xc
	UCHAR TraceFlags;                                                       //0xd
	UCHAR InfoMask;                                                         //0xe
	UCHAR Flags;                                                            //0xf
	union
	{
		struct _OBJECT_CREATE_INFORMATION* ObjectCreateInfo;                //0x10
		VOID* QuotaBlockCharged;                                            //0x10
	};
	VOID* SecurityDescriptor;                                               //0x14
	struct _QUAD Body;                                                      //0x18
}OBJECT_HEADER,*POBJECT_HEADER;

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver,PUNICODE_STRING pReg) {
	pDriver->DriverUnload = DriverUnload;

	ULONG_PTR *pPspCidTable = NULL;
	ULONG_PTR *pObTypeIndexTable = NULL;
	PHANDLE_TABLE pGHandleTable = NULL;
	PHANDLE_TABLE_ENTRY TableCodeAddr = NULL;
	UINT32 TableCount = 0;
	UINT32 index = 0;
	PEPROCESS pEProcess = NULL;
	PUNICODE_STRING pUni_ProcName = NULL;
	PWCH pwch = NULL;

	pPspCidTable = *(ULONG_PTR*)((PUCHAR)PsLookupProcessByProcessId + 0x20);
	pObTypeIndexTable = *(ULONG_PTR*)((PUCHAR)ObGetObjectType + 0xf);
	KdPrintEx((77,0,"PspCidTable:%08x\tObTypeIndexTable:%08x\n", *pPspCidTable, pObTypeIndexTable));
	pGHandleTable = (PHANDLE_TABLE)*pPspCidTable;
	TableCount = pGHandleTable->TableCode & 0x00000003; // 低两位
	//KdPrintEx((77,0,"%x\n", TableCodeAddr));


	//KdBreakPoint();
	for (int i = 0; i <= TableCount; i++) {
		TableCodeAddr = ((PHANDLE_TABLE_ENTRY*)(pGHandleTable->TableCode & 0xfffffffe))[i];
		index = 0;
		// 遍历第i张表
		while (1) {
			// 判断是否超过表索引
			if (index > (PAGE_SIZE/(sizeof(ULONG_PTR))*2)) break;
			if (TableCodeAddr[index].Object == NULL) {
				index++;
				continue;
			}
			// 校验地址是否有效
			if (MmIsAddressValid(TableCodeAddr[index].Object) == FALSE) break;

			pEProcess = (PUCHAR)(((UINT32)TableCodeAddr[index].Object) & 0xfffffff8);
			// win7x32 类型索引7是进程，8是线程
			if (((POBJECT_HEADER)((PUCHAR)pEProcess - 0x18))->TypeIndex == 0x7) {
				// SeLocateProcessImageName无法获取内核文件全路径(获取全路径，便于输入中文字符)
				// 该函数只是浅拷贝，并没有复制内存
				NTSTATUS status = SeLocateProcessImageName(pEProcess,&pUni_ProcName);
				if (!NT_SUCCESS(status)) {
					index++;
					continue;
				};

				if (MmIsAddressValid(pUni_ProcName->Buffer)==FALSE) {
					KdPrintEx((77, 0, "Pid:%d,ImageName:%s\n", PsGetProcessId(pEProcess), (PUCHAR)pEProcess+0x16c));

				}else{
					pwch = wcsrchr(pUni_ProcName->Buffer, L'\\');
					if (pwch == NULL) {
						index++;
						continue;
					};
					pwch += 1;
					KdPrintEx((77, 0, "Pid:%d,ImageName:%S\n", PsGetProcessId(pEProcess), pwch));
				}
				pwch = NULL;
			}
			index++;
		}
	}


	return STATUS_SUCCESS;
}