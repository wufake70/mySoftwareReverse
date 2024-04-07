#include "stdafx.h"
#include "petool.h"
#include "petool2.h"

#define _CRT_SECURE_NO_WARNINGS

int main(int argc, char* argv[])
{

	// Check Resource table
	BYTE log_code=0;
	char file_path[1000];
	char* file_buffer=NULL;
	size_t index=0;

	printf("Put a exe path: ");
	scanf_s("%s",file_path,sizeof(file_path));

	file_buffer=ReadPE(file_path);
	if(file_buffer)
	{
		log_code++;
		CheckResourceTable(file_buffer);
		printf("Please put a index of resource needed for search.\n");
		while(1)
		{
			printf(": ");
			scanf_s("%d",&index);
			CheckResourceInfo(file_buffer,index);
		}
	}


	system("pause>nul");
	return 0;
}
