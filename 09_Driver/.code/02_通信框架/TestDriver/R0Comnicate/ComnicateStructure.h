#pragma once
#ifdef _R3
#include <Windows.h>
#else
#include <ntifs.h>
#endif

#define _COMM_ID	0x12345678	// 通信id
#define SYM_NAME    L"\\??\\hackflame1"

// 设备控制码
typedef enum _CMD
{
	//TEST = 0,
	CODE_READ = 0X1,
	CODE_WRITE = 0X2,


}CMD;

// io包(类似IRP包)
typedef struct _CommPackage
{
	ULONG64 id;
	ULONG64 code;
	ULONG64 inData;
	ULONG64 inLen;
	ULONG64 outData;
	ULONG64 outLen;
}CommPackage, *PCommPackage;

typedef struct _Test
{
	int x;
}Test, *PTest;