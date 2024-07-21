#include "R3Comnicate/R3Comnicate.h"


int main()
{
	printf("test CODE_WRITE\n");
	system("pause>nul");
	Test x1 = { 0 };
	x1.x = 0x100;
	DriverComm(CODE_WRITE, &x1, 4, NULL, NULL);


	printf("test CODE_READ: ");
	system("pause>nul");
	DriverComm(CODE_READ, &x1, 4, NULL, NULL);
	printf("%x\r\n", x1.x);
	system("pause");

	return 0;
}

