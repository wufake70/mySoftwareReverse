#include <stdio.h>

void HelloWorld()
{

	int i=0;
	int a[]={1,2,3,4,5,6,7,8,9,10};
	for(i=0;i<=10;i++){
		a[i]=0;		// 该程序进入死循环原因
					// a[10](已经数组溢出)和局部变量i在缓冲区是同一个位置(EBP-4),
					// a[10]=0;实际上就是i=0,无法跳出for循环
		printf("Hello World!\n");
	}

}
int main(int argc,char* argv[])
{
	HelloWorld();
	getchar();
	return 0;
}