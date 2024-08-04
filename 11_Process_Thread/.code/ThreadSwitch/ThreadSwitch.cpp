// ThreadSwitch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ThreadCore.h"
extern int CurrentThreadindex ;
int main(int argc, char* argv[])
{
	//初始化线程环境
	RegisterGMThread("Thread1",Thread1,NULL);
	RegisterGMThread("Thread2",Thread2,NULL);
	RegisterGMThread("Thread3",Thread3,NULL);
	RegisterGMThread("Thread4",Thread4,NULL);

    //仿Windows线程切换
	for (;;)
	{
		Sleep(20);
		Scheduling();
	}
	
	return 0;
}