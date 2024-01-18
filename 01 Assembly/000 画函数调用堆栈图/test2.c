# include <stdio.h>

void Attack()
{
	while(1)
	{
		printf("攻击程序\n");

	}
	getchar();
	
}

int main(int argc,char* argv[])
{

	int arr[5]={0};
	arr[6]=(int)Attack;
	/*
		陷入循环的原因
			arr[6]为ebp+4即main函数的返回地址，
			被修改为Attack的地址
			执行返回地址即执行attack函数
	*/
	return 0;
}



