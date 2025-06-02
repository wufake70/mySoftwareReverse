#include <cstdio>
#include <Windows.h>
#include <intrin.h>

//特性            |SEH（TEB 异常链表）                            |VEH
//存储位置        |线程栈（FS:[0]/GS:[0]）                        |全局链表（ntdll!RtlpCalloutEntryList）
//作用范围        |仅当前线程                                     |所有线程
//调用顺序        |在 VEH 之后（默认）                            |在 SEH 之前（默认）
//注册方式        |__try/__except 或手动 RtlAddFunctionTable     |AddVectoredExceptionHandler()
//是否影响调试    | 调试器可以覆盖 SEH                            |调试器通常先于 VEH 捕获异常

LONG NTAPI VectoredExceptionHandler(
	struct _EXCEPTION_POINTERS* ExceptionInfo
	)
{

	if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		printf("ExceptionCode: 0x%x\n", ExceptionInfo->ExceptionRecord->ExceptionCode);
#ifdef _WIN64
		ExceptionInfo->ContextRecord->Rip += 8;
#else
		ExceptionInfo->ContextRecord->Eip += 7;
#endif // _WIN64
		return EXCEPTION_CONTINUE_EXECUTION;
	}

		return EXCEPTION_CONTINUE_SEARCH;

}

union U1
{
    ULONG CrossProcessFlags;                                            //0x28
    struct
    {
        ULONG ProcessInJob : 1;                                           //0x28
        ULONG ProcessInitializing : 1;                                    //0x28
        ULONG ProcessUsingVEH : 1;                                        //0x28
        ULONG ProcessUsingVCH : 1;                                        //0x28
        ULONG ProcessUsingFTH : 1;                                        //0x28
        ULONG ProcessPreviouslyThrottled : 1;                             //0x28
        ULONG ProcessCurrentlyThrottled : 1;                              //0x28
        ULONG ProcessImagesHotPatched : 1;                                //0x28
        ULONG ReservedBits0 : 24;                                         //0x28
    };
};

int main()
{
#ifdef _WIN64
	LONG_PTR ppeb = (LONG_PTR)__readgsqword(0x60);
	U1* u1 = (U1*)(((ULONG64)ppeb) + 0x50);
#else
	LONG_PTR ppeb = (LONG_PTR)__readfsdword(0x30);
	U1* u1 = (U1*)(((DWORD)ppeb) + 0x28);

#endif
	// 观察 ProcessUsingVEH标志位
	{
		printf("添加VEH前 ProcessUsingVEH:0x%x\n", u1->ProcessUsingVEH);

		// VEH添加成功
		PVOID res =  AddVectoredExceptionHandler(TRUE, VectoredExceptionHandler);

		// ** 将该ProcessUsingVEH置为0，VEH不处理异常 **
		u1->ProcessUsingVEH = 0;

		if(res)
			printf("添加VEH成功 ProcessUsingVEH:0x%x\n", u1->ProcessUsingVEH);
		else 
		{
			printf("添加VEH失败\n");
			return 0;
		}

		__try {

		*(byte*)0 = 1;
		}
		__except(1){
			printf("SEH接管\n");
		}
		printf("VEH处理后 ProcessUsingVEH:0x%x\n", u1->ProcessUsingVEH);
	}

	printf("It's Run\n");
	system("pause>nul");

	return 0;
}
