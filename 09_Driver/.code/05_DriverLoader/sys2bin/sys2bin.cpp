#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

int main(int argc, char * args[], char **env)
{
	if (argc < 2)
	{
		printf("参数不对\r\n");
		printf("举个例子:build.exe dll路径\r\n");
		return 0;
	}

	char * dllpath = args[1];
	FILE * file = NULL;
	fopen_s(&file, dllpath, "rb");

	if (!file)
	{
		printf("文件不存在\r\n");
		return 0;
	}

	//求文件大小
	fseek(file, 0, SEEK_END);
	unsigned int len = ftell(file);

	//绕回到头部文件流指针
	rewind(file);

	unsigned char * filebuffer = (unsigned char *)malloc(len);
	memset(filebuffer, 0, len);

	fread_s(filebuffer, len, len, 1, file);

	fclose(file);

	//创建一个文件 写入我们的硬编码
	if (argc == 2)
	{
		fopen_s(&file, "dll.h", "wb");
	}
	else
	{
		fopen_s(&file, args[2], "wb");
	}

	if (file == NULL)
	{
		free(filebuffer);
		return 0;
	}

	fputs("#pragma once\n", file);

	fprintf_s(file, "unsigned char sysData[%d] = 	{\n", len);
	fprintf_s(file, "\t");
	for (int i = 0; i < len; i++)
	{
		filebuffer[i] ^= 0x73;
		filebuffer[i] ^= 0xde;
		fprintf_s(file, "0x%02X, ", filebuffer[i]);
		if ((i + 1) % 30 == 0)
		{
			fprintf_s(file, "\r\n\t");
		}


	}

	fprintf_s(file, "\r\n};");
	fclose(file);

	free(filebuffer);
	return 0;
}

