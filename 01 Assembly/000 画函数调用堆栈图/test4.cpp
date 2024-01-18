#include <stdio.h>

typedef int(*pFunction)(int,int);

int Plus(int x,int y)
{
	return x+y;
}


int main(int argc,char* argv[])
{
	int z=Plus(1,2);
	printf("%d\n",z);
	pFunction p= (pFunction)Plus;
	int m=p(4,5);
	printf("%d\n",m);
	getchar();
	return 0;
}