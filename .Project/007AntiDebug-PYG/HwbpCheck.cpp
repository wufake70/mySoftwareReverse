#include "HwbpCheck.h"
#include "ntdll/ntdll.h"
#include <cstdio>

/*
检测硬件断点
*/
BOOL HwbpCheck()
{
	CONTEXT		ctx;
	ctx.ContextFlags = CONTEXT_ALL;

	GetThreadContext(NtCurrentThread,
		&ctx);
	if (ctx.Dr0 || ctx.Dr1 || ctx.Dr3 || ctx.Dr2) 
		return TRUE;
	else 
		return FALSE;
}



LONG NTAPI VEHandle(PEXCEPTION_POINTERS ExceptionInfo)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if (ExceptionInfo->ContextRecord->Dr0 ||
			ExceptionInfo->ContextRecord->Dr1 ||
			ExceptionInfo->ContextRecord->Dr2 ||
			ExceptionInfo->ContextRecord->Dr3)
			printf("有硬件断点(VEH)\n");

		else
			printf("无硬件断点(VEH)\n");


#ifdef _WIN64
		ExceptionInfo->ContextRecord->Rip += 8;	// 需要修正Rip地址
#else
		ExceptionInfo->ContextRecord->Eip += 7;
#endif _WIN64
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;

}

VOID HwbpCheckByVEH()
{
	AddVectoredExceptionHandler(0, VEHandle);
	// 触发异常
	*(BYTE*)0=0;
	RemoveVectoredExceptionHandler(VEHandle);

}