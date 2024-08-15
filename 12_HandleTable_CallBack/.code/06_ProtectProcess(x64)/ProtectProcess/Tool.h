#pragma once
#include <ntifs.h>

#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020)  
#define PROCESS_ALL_ACCESS					(0xffff)



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
}HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;

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
}HANDLE_TABLE, *PHANDLE_TABLE;

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
}OBJECT_HEADER, *POBJECT_HEADER;

typedef BOOLEAN(NTAPI * PEX_ENUM_HANDLE_CALLBACK) (_In_ struct _HANDLE_TABLE_ENTRY *HandleTableEntry, _In_ HANDLE Handle, _In_ PVOID Paramter);

NTKERNELAPI BOOLEAN ExEnumHandleTable(
	IN  PHANDLE_TABLE HandleTable,
	IN  PEX_ENUM_HANDLE_CALLBACK EnumHandleProcedure,
	IN  PVOID EnumParameter,
	OUT PHANDLE Handle OPTIONAL
);

PEPROCESS FindProcessByNameW(PWCH processName);
PEPROCESS FindProcessByNameA(char* processName);
VOID ForceEnumProcess(PCHAR WhiteList[], PEPROCESS TagetProcess);
VOID SetHandleAccess(PEPROCESS srcProcess, PEPROCESS targetProcess);
BOOLEAN NTAPI EnumHandleProcedure(PHANDLE_TABLE_ENTRY pHandleTableEntry, HANDLE Handle, PVOID Paramter);