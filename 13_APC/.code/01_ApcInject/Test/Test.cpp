// Test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>

int main()
{

	while (1)
	{
		printf("�ں˴���\r\n");
		SleepEx(1000, TRUE); // ��Ҫʹ��Sleep��������api�Ὣkthread.Alertable��Ϊ0���� ���ɱ����ѣ�
				  // apc�޷�����
	}

    return 0;
}

