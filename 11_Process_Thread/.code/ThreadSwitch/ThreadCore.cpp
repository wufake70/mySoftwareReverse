
#include "stdafx.h"
#include "stdio.h"
#include "windows.h"

#include "ThreadCore.h"

#define _SELF		"滴水_仿Windows线程切换"

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

int CurrentThreadindex = 0;
GMThread_t GMThreadList[MAXGMTHREAD] = { NULL,0 };

#define GMTHREADSTACKSIZE 0x80000

void *WindowsStackLimit = NULL;

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

__declspec(naked) void SwitchContext(GMThread_t *SrcGMThreadp,GMThread_t *DstGMThreadp)
{
	__asm
	{
		push ebp
		mov ebp,esp
		//sub esp,__LOCAL_SIZE
		push edi
		push esi
		push ebx
		push ecx
		push edx
		push eax
		
		mov esi,SrcGMThreadp
		mov edi,DstGMThreadp

		mov [esi+GMThread_t.KernelStack],esp
		//---------------经典堆栈切换 另一个线程复活----------------------------------
		mov esp,[edi+GMThread_t.KernelStack]

		
		pop eax
		pop edx
		pop ecx
		pop ebx
		pop esi
		pop edi
		//add esp,__LOCAL_SIZE
		pop ebp
		ret
	}
}


void GMThreadStartup(GMThread_t *GMThreadp)
{
	GMThreadp->func(GMThreadp->lpParameter);
	GMThreadp->Flags = GMTHREAD_EXIT;
	Scheduling();

	return ;
}

void IdleGMThread(void *lpParameter)
{
	printf("IdleGMThread---------------\n");
	Scheduling();
	return ;
}

void PushStack(unsigned int **Stackpp,unsigned int v)
{
	*Stackpp -= 1;
	**Stackpp = v;

	return ;
}

void initGMThread(GMThread_t *GMThreadp,char *name,void (*func)(void *lpParameter),void *lpParameter)
{
	unsigned char *StackPages;
	unsigned int *StackDWORDParam;
    //结构初始化赋值
	GMThreadp->Flags = GMTHREAD_CREATE;

	GMThreadp->name = name;
	GMThreadp->func = func;
	GMThreadp->lpParameter = lpParameter;
    //申请空间
	StackPages = (unsigned char*)VirtualAlloc(NULL,GMTHREADSTACKSIZE,MEM_COMMIT,PAGE_READWRITE);
	//清零
	memset(StackPages,0,GMTHREADSTACKSIZE);
	//堆栈初始化地址
	GMThreadp->InitialStack = (StackPages+GMTHREADSTACKSIZE);
	//堆栈限制
	GMThreadp->StackLimit = StackPages;
	//堆栈地址
	StackDWORDParam = (unsigned int*)GMThreadp->InitialStack;
	
	//入栈
	PushStack(&StackDWORDParam,(unsigned int)GMThreadp);		//通过这个指针来找到：线程函数、函数参数
	PushStack(&StackDWORDParam,(unsigned int)9);				//平衡堆栈
	PushStack(&StackDWORDParam,(unsigned int)GMThreadStartup);	//线程入口函数 这个函数负责调用线程函数
	PushStack(&StackDWORDParam,5);								//push ebp
	PushStack(&StackDWORDParam,7);								//push edi
	PushStack(&StackDWORDParam,6);								//push esi
	PushStack(&StackDWORDParam,3);								//push ebx
	PushStack(&StackDWORDParam,2);								//push ecx
	PushStack(&StackDWORDParam,1);								//push edx
	PushStack(&StackDWORDParam,0);								//push eax
    
	GMThreadp->KernelStack = StackDWORDParam;
	

	GMThreadp->Flags = GMTHREAD_READAY;

	return ;
}

int RegisterGMThread(char *name,void (*func)(void *lpParameter),void *lpParameter)
{
	int i;

	for (i=1;GMThreadList[i].name;i++)
	{
		if (0==stricmp(GMThreadList[i].name,name))
		{
			break;
		}
	}
	initGMThread(&GMThreadList[i],name,func,lpParameter);

	return (i|0x55AA0000);
}

void Scheduling(void)
{
	int i;
	int TickCount;
	GMThread_t *SrcGMThreadp;
	GMThread_t *DstGMThreadp;

	TickCount = GetTickCount();

	SrcGMThreadp = &GMThreadList[CurrentThreadindex];
	
	DstGMThreadp = &GMThreadList[0];
	// 查找已经初始化的线程
	for (i=1;GMThreadList[i].name;i++)
	{
		if (GMThreadList[i].Flags&GMTHREAD_SLEEP)
		{
			if (TickCount>GMThreadList[i].SleepMillisecondDot)
			{
				GMThreadList[i].Flags = GMTHREAD_READAY;
			}
		}

		if ((GMThreadList[i].Flags&GMTHREAD_READAY))
		{
			DstGMThreadp = &GMThreadList[i];
			break;
		}
	}
	
	CurrentThreadindex = DstGMThreadp-GMThreadList;
	SwitchContext(SrcGMThreadp,DstGMThreadp);
	
	return ;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void GMSleep(int Milliseconds)
{
	GMThread_t *GMThreadp;
	GMThreadp = &GMThreadList[CurrentThreadindex];

	if ((GMThreadp->Flags)!=0)
	{
		GMThreadp->SleepMillisecondDot = GetTickCount()+Milliseconds;
		GMThreadp->Flags = GMTHREAD_SLEEP;
	}

	Scheduling();
	return ;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void vmmprint(char *f,int l,char *fmt, ...)
{
	int ret;
	char buffer[0x100];
	va_list args;
	
	//----------------------------------
	va_start(args, fmt);
	_snprintf(buffer,0x80,"[%s]:",f,l);
	ret = _vsnprintf(buffer+strlen(buffer), 0x100-strlen(buffer), fmt, args);
	if (ret == -1)
	{
		strcpy(buffer, "vmmprint: error.");
	}
	//----------------------------------
	printf("%s",buffer);
	//OutputDebugString(buffer);
	
	return ;
}

void Thread1(void *lpParameter)
{
	int i;
	for (i=0;i<3;i++)
	{
		vmmprint(_SELF,__LINE__,"Thread1 \n");
		GMSleep(1000);
	}

	return ;
}

void Thread2(void *lpParameter)
{
	for (;;)
	{
		vmmprint(_SELF,__LINE__,"Thread2 \n");
		GMSleep(500);
	}
	
	return ;
}

void Thread3(void *lpParameter)
{
	for (;;)
	{
		vmmprint(_SELF,__LINE__,"Thread3 \n");
		GMSleep(800);
	}
	
	return ;
}

void Thread4(void *lpParameter)
{
	
	for (;;)
	{
		vmmprint(_SELF,__LINE__,"Thread4 \n");
		GMSleep(200);
	}
	
	return ;
}
