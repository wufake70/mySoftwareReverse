// ThreadCore.h: interface for the ThreadCore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADCORE_H__3C5DBE32_012F_4176_884F_8D9EA510122D__INCLUDED_)
#define AFX_THREADCORE_H__3C5DBE32_012F_4176_884F_8D9EA510122D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#define MAXGMTHREAD	0x100


#define GMTHREAD_CREATE		0x01
#define GMTHREAD_READAY		0x02
#define GMTHREAD_RUNING		0x04
#define GMTHREAD_SLEEP		0x08
#define GMTHREAD_EXIT		0x100


//线程结构体(仿THREAD)
typedef struct
{
	char *name;						//线程名 相当于线程TID
	int Flags;						//线程状态
	int SleepMillisecondDot;		//休眠时间
	
	void *InitialStack;				//线程堆栈起始位置
	void *StackLimit;				//线程堆栈界限
	void *KernelStack;				//线程堆栈当前位置,也就是ESP
	
	void *lpParameter;				//线程函数的参数
	void (*func)(void *lpParameter);//线程函数
	
} GMThread_t;


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

extern GMThread_t GMThreadList[MAXGMTHREAD];//线程结构体数组

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------


void IdleGMThread(void *lpParameter);

void initGMThread(GMThread_t *GMThreadp,char *name,void (*func)(void *lpParameter),void *lpParameter);
int RegisterGMThread(char *name,void (*func)(void *lpParameter),void *lpParameter);
void Scheduling(void);

void GMSleep(int Milliseconds);

void Thread1(void *lpParameter);
void Thread2(void *lpParameter);
void Thread3(void *lpParameter);
void Thread4(void *lpParameter);


#endif // !defined(AFX_THREADCORE_H__3C5DBE32_012F_4176_884F_8D9EA510122D__INCLUDED_)
