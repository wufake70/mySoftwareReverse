// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <Windows.h>
#include "LoadPE.h"
#include "LoadPE2.h"


int main()
{
	CHAR szPathSrc[MAX_PATH] = { 0 };
	CHAR szPath[MAX_PATH] = {0};
	UINT NewImgBase = 0;

	printf("Src Exe Path: ");
	scanf_s("%s",szPathSrc,MAX_PATH);
	printf("new imgbase: ");
	scanf_s("%x", &NewImgBase);

    HANDLE hFile = CreateFileA(szPathSrc, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    DWORD dwFileSize = GetFileSize(hFile, NULL); //获取替换可执行文件的大小
    LPVOID FileImage = VirtualAlloc(NULL, dwFileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!ReadFile(hFile, FileImage, dwFileSize, NULL, NULL))
	{
		CloseHandle(hFile);
		return 0;
	}
    CloseHandle(hFile);
	hFile = NULL;

	memcpy(szPath,szPathSrc,strlen(szPathSrc)-4);
	sprintf_s(szPath,"%s_fixed.exe",szPath);
	RepairByRelocTable((LPBYTE)FileImage,NewImgBase);
	hFile = CreateFileA(szPath, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	WriteFile(hFile,FileImage,dwFileSize,NULL,NULL);
    CloseHandle(hFile);
    system("pause");
}




