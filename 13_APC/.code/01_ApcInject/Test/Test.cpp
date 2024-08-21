// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

int main()
{

	while (1)
	{
		printf("内核达人\r\n");
		SleepEx(1000, TRUE); // 不要使用Sleep函数，该api会将kthread.Alertable置为0，即 不可被唤醒，
				  // apc无法触发
	}

    return 0;
}

