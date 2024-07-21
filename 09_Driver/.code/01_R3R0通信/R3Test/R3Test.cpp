#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <WinIoCtl.h>  // io通信

#define SYMBL_LINK_NAME L"\\\\.\\mydevice"

// 设备控制码
#define CODE_READ CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define CODE_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN,0x900,METHOD_BUFFERED,FILE_ANY_ACCESS)


int main()
{
	// 读取符号文件
	HANDLE hFile = CreateFile(SYMBL_LINK_NAME, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	char* memBuffer = (char*)malloc(0x20);
	int memLen = 0;
	memset(memBuffer, 0, 0x20);
	printf("read test\n");
	system("pause>nul");
	/*
	BOOL WINAPI	DeviceIoControl(
    _In_ HANDLE hDevice,
    _In_ DWORD dwIoControlCode,			设备控制码
    _In_reads_bytes_opt_(nInBufferSize) LPVOID lpInBuffer,	// 输入缓冲区
    _In_ DWORD nInBufferSize,
    _Out_writes_bytes_to_opt_(nOutBufferSize,*lpBytesReturned) LPVOID lpOutBuffer,	// 输出缓冲区
    _In_ DWORD nOutBufferSize,
    _Out_opt_ LPDWORD lpBytesReturned,
    _Inout_opt_ LPOVERLAPPED lpOverlapped	是否异步执行,NULL 是同步会阻塞当前线程
    );
	*/
	DeviceIoControl(hFile, CODE_READ, memBuffer, 0x20, memBuffer, 0x20, (LPDWORD)&memLen, (LPOVERLAPPED)NULL);
	printf("CODE_READ: %s\n", memBuffer);

	printf("write test\n");
	system("pause>nul");
	memset(memBuffer, 0, 0x20);
	sprintf_s(memBuffer, 0x20, "%s", "helloworld");
	DeviceIoControl(hFile, CODE_WRITE, memBuffer, 0x20, memBuffer, 0x20, (LPDWORD)&memLen, (LPOVERLAPPED)NULL);
	CloseHandle(hFile);


	system("pause>nul");
	CloseHandle(hFile);

}


