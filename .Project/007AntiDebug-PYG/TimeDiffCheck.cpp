#include "TimeDiffCheck.h"
#include "ntdll/ntdll.h"
#include <cstdio>

BOOL TimeDiffCheck1()
{
	DWORD start = GetTickCount();
	__try {
		DbgBreakPoint();
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// EXCEPTION_BREAKPOINT 0x80000003
		printf("ExceptionCode(int3断点异常): 0x%x\n",GetExceptionCode());
	}
	DWORD end = GetTickCount();
	return end - start > 100 ? TRUE : FALSE;
}

BOOL TimeDiffCheck2()
{
	unsigned __int64 t1, t2;
	t1 = __rdtsc();        // 第一次读取 TSC
	__try {
		int x = 0;
		int y = 9 / x;
	}
	__except(EXCEPTION_EXECUTE_HANDLER){
		// STATUS_INTEGER_DIVIDE_BY_ZERO 0xc0000094
		printf("ExceptionCode(除零异常): 0x%x\n", GetExceptionCode());
	}
	t2 = __rdtsc();        // 第二次读取 TSC

	// 计算时间差（CPU时钟周期数）
	unsigned __int64 diff = t2 - t1;

	// 阈值设定（需根据实际硬件调整）
	const unsigned __int64 THRESHOLD = 1000000000ULL; // 例如 1e9 周期

	return (diff > THRESHOLD); // 时间差过大则判定调试
}